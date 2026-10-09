// Description: Standard shader for SAGE

#define MAX_SPOT_LIGHTS 16
#define MAX_SHADOW_SAMPLES 64 // Upper bound for the runtime sample counts. Must match StandardEffect::MaxShadowSamples

struct SpotLightData
{
    float3 position;
    float range;
    float3 direction;
    float innerConeAngle;
    float3 attenuation;
    float outerConeAngle;
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp[2];
    float3 viewPosition;
    float bumpWeight; //Padding which we can use for the Bump Multiplyer
}

cbuffer BoneTransformBuffer : register(b1)
{
    matrix boneTransforms[128];
}

cbuffer LightBuffer : register(b2)
{
    float3 lightDirection;
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
}

cbuffer MaterialBuffer : register(b3)
{
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float4 materialEmissive;
    float materialPower;
}

cbuffer SettingBuffer : register(b4)
{
    bool useDiffuseMap;
    bool useSpecularMap;
    bool useBumpMap;
    bool useNormalMap;

    bool useShadowMap;
    bool useSkinning;
    bool useFog;
    float depthBias;

    float4 fogColor;

    float fogStart;
    float fogEnd;
    float2 tiling;

    float2 tilingOffset;
    float2 settingPadding;
}

cbuffer SpotLightBuffer : register(b5)
{
    SpotLightData spotLights[MAX_SPOT_LIGHTS];
    int spotLightCount;
    int spotLightShadowMask;
    float2 spotLightPadding;
}

cbuffer SpotLightMatrixBuffer : register(b6)
{
    matrix spotLightViewProj[MAX_SPOT_LIGHTS];
}

cbuffer ShadowSettingBuffer : register(b7)
{
    int pcfSampleCount; // taps for the final filter (16-32 looks great)
    int blockerSampleCount; // taps for the PCSS blocker search
    float pcfRadiusTexels; // spot light filter radius
    float pcssLightSize; // penumbra texels per unit of NDC depth difference

    float pcssSearchRadiusTexels; // how far to look for blockers
    float pcssMinRadiusTexels; // sharpest allowed shadow edge
    float pcssMaxRadiusTexels; // softest allowed shadow edge
    float kernelBiasScale; // extra bias multiplier per texel of filter radius

    float spotNormalOffset; // world units to push spot shadow lookups along the normal
    float3 shadowSettingPadding;
}

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D bumpMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D shadowMap : register(t4);
Texture2D spotShadowMaps[MAX_SPOT_LIGHTS] : register(t5);

SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    int4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHT;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 worldPosition : TEXCOORD4;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float3 dirToLight : TEXCOORD0;
    float3 dirToView : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
    float4 lightNDCPosition : TEXCOORD3;
    float fogFactor : FOG;
};

static matrix Identity =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

static const float TWO_PI = 6.28318530718f;

matrix GetBoneTransform(int4 indices, float4 weights)
{
    if (length(weights) <= 0.0f)
    {
        return Identity;
    }
    
    matrix transform;
    transform = boneTransforms[indices[0]] * weights[0];
    transform += boneTransforms[indices[1]] * weights[1];
    transform += boneTransforms[indices[2]] * weights[2];
    transform += boneTransforms[indices[3]] * weights[3];
    return transform;
}

// ---------------------------------------------------------------------------
// Shadow filtering
// ---------------------------------------------------------------------------

// Cheap, high quality per-pixel noise (Jimenez). Needs SV_Position.xy
float InterleavedGradientNoise(float2 pixelPos)
{
    return frac(52.9829189f * frac(dot(pixelPos, float2(0.06711056f, 0.00583715f))));
}

// Evenly distributed disk samples; rotation decorrelates neighboring pixels
float2 VogelDiskSample(int index, int count, float rotation)
{
    const float goldenAngle = 2.39996323f;
    const float r = sqrt((float) index + 0.5f) / sqrt((float) count);
    const float theta = (float) index * goldenAngle + rotation;
    float s, c;
    sincos(theta, s, c);
    return float2(c, s) * r;
}

float2 GetTextureSize(Texture2D tex)
{
    uint w, h;
    tex.GetDimensions(w, h);
    return float2(w, h);
}

// Converts light-space clip pos to shadow map UV + depth. Returns false if outside frustum.
bool GetShadowCoords(float4 lightNDCPosition, out float2 uv, out float depth)
{
    uv = 0.0f;
    depth = 0.0f;
    if (lightNDCPosition.w <= 0.0f)
    {
        return false;
    }

    const float3 p = lightNDCPosition.xyz / lightNDCPosition.w;
    depth = 1.0f - p.z;
    uv = float2(p.x * 0.5f + 0.5f, 0.5f - p.y * 0.5f);
    return all(saturate(uv) == uv);
}

// Point-sampled depth fetch. Returns false if the tap falls outside the map.
bool LoadShadowDepth(Texture2D shadowTex, float2 uv, float2 dim, out float savedDepth)
{
    savedDepth = 0.0f;
    if (any(uv < 0.0f) || any(uv >= 1.0f))
    {
        return false;
    }
    
    savedDepth = shadowTex.Load(int3(uv * dim, 0)).r;
    return true;
}

// Slope-scaled bias that also grows with the filter radius (wide kernels need more bias)
float ComputeShadowBias(float bias, float NdotL, float radiusTexels)
{
    const float nl = saturate(NdotL);
    const float slope = clamp(sqrt(1.0f - nl * nl) / max(nl, 0.0001f), 0.0f, 8.0f);
    return bias * (1.0f + slope) * (1.0f + radiusTexels * kernelBiasScale);
}

// Rotated Vogel-disk PCF. Returns 0 (shadowed) .. 1 (lit)
float FilterShadow(Texture2D shadowTex, float2 uv, float receiverDepth, float effectiveBias,
                   float radiusTexels, float2 dim, float rotation)
{
    const int sampleCount = clamp(pcfSampleCount, 1, MAX_SHADOW_SAMPLES);
    const float2 radiusUV = radiusTexels / dim;
    float lit = 0.0f;

    [loop]
    for (int i = 0; i < sampleCount; ++i)
    {
        const float2 sampleUV = uv + VogelDiskSample(i, sampleCount, rotation) * radiusUV;
        float savedDepth;
        if (!LoadShadowDepth(shadowTex, sampleUV, dim, savedDepth))
        {
            lit += 1.0f; // outside the map: treat as lit
            continue;
        }
        lit += (savedDepth > receiverDepth + effectiveBias) ? 0.0f : 1.0f;
    }
    return lit / sampleCount;
}

// Fixed-radius randomized PCF (used for spot lights)
float ComputeShadowFactorPCF(Texture2D shadowTex, float4 lightNDCPosition, float bias, float NdotL,
                             float2 pixelPos, float rotationOffset)
{
    float2 uv;
    float depth;
    if (!GetShadowCoords(lightNDCPosition, uv, depth))
    {
        return 1.0f; // outside frustum: fully lit
    }

    const float2 dim = GetTextureSize(shadowTex);
    const float rotation = InterleavedGradientNoise(pixelPos) * TWO_PI + rotationOffset;
    const float effectiveBias = ComputeShadowBias(bias, NdotL, pcfRadiusTexels);
    return FilterShadow(shadowTex, uv, depth, effectiveBias, pcfRadiusTexels, dim, rotation);
}

// PCSS: blocker search -> penumbra estimate -> variable-radius PCF (orthographic light only)
float ComputeShadowFactorPCSS(Texture2D shadowTex, float4 lightNDCPosition, float bias, float NdotL,
                              float2 pixelPos)
{
    float2 uv;
    float depth;
    if (!GetShadowCoords(lightNDCPosition, uv, depth))
    {
        return 1.0f;
    }

    const float2 dim = GetTextureSize(shadowTex);
    const float rotation = InterleavedGradientNoise(pixelPos) * TWO_PI;

    // 1) Blocker search
    const int searchCount = clamp(blockerSampleCount, 1, MAX_SHADOW_SAMPLES);
    const float searchBias = ComputeShadowBias(bias, NdotL, pcssSearchRadiusTexels);
    const float2 searchRadiusUV = pcssSearchRadiusTexels / dim;
    float blockerSum = 0.0f;
    int blockerCount = 0;

    [loop]
    for (int i = 0; i < searchCount; ++i)
    {
        const float2 sampleUV = uv + VogelDiskSample(i, searchCount, rotation) * searchRadiusUV;
        float savedDepth;
        if (LoadShadowDepth(shadowTex, sampleUV, dim, savedDepth) && savedDepth > depth + searchBias)
        {
            blockerSum += savedDepth;
            blockerCount++;
        }
    }

    if (blockerCount == 0)
    {
        return 1.0f; // nothing nearby blocks the light: fully lit, skip the filter
    }

    // 2) Penumbra estimate: farther blocker = wider, softer edge
    const float avgBlockerDepth = blockerSum / blockerCount;
    const float radiusTexels = clamp((avgBlockerDepth - depth) * pcssLightSize, pcssMinRadiusTexels, pcssMaxRadiusTexels);

    // 3) Filter with the estimated radius
    const float effectiveBias = ComputeShadowBias(bias, NdotL, radiusTexels);
    return FilterShadow(shadowTex, uv, depth, effectiveBias, radiusTexels, dim, rotation);
}

// ---------------------------------------------------------------------------
// Lighting
// ---------------------------------------------------------------------------

float4 ComputeSpotLightContribution(int index, float3 worldPosition, float3 normal, float3 viewDirection,
                                    float4 diffuseMapColor, float specularMapColor, float2 pixelPos)
{
    SpotLightData light = spotLights[index];
    const float3 toLight = light.position - worldPosition;
    const float dist = length(toLight);

    if (dist <= 0.0001f)
    {
        return 0.0f;
    }

    const float3 spotL = toLight / dist;
    const float cosAngle = dot(-spotL, normalize(light.direction));
    const float spotFactor = smoothstep(cos(light.outerConeAngle), cos(light.innerConeAngle), cosAngle);

    if (spotFactor <= 0.0f)
    {
        return 0.0f;
    }

    const float attenuation = light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist;
    const float atten = spotFactor / max(attenuation, 0.0001f);
    const float diffuseAmount = saturate(dot(spotL, normal));
    const float3 reflection = reflect(-spotL, normal);
    const float specularAmount = pow(saturate(dot(reflection, viewDirection)), materialPower);

    float shadowFactor = 1.0f;
    if ((spotLightShadowMask & (1 << index)) != 0) // Can Casts Shadow
    {
        // Push the shadow test point along the normal before sampling. Compensates
        // for curvature-induced self-shadowing that a depth-only bias can't fix.
        const float normalOffsetScale = 0.01f; // tune per scene scale; start small and increase until acne clears
        const float3 shadowSamplePos = worldPosition + normal * normalOffsetScale;
        const float4 spotNDC = mul(float4(shadowSamplePos, 1.0f), spotLightViewProj[index]);
        shadowFactor = ComputeShadowFactorPCF(spotShadowMaps[index], spotNDC, depthBias, diffuseAmount,
                                              pixelPos, index * 1.7f); // offset so lights get different noise
    }

    const float4 ambient = light.ambient * materialAmbient;
    const float4 diffuse = diffuseAmount * light.diffuse * materialDiffuse * shadowFactor;
    const float4 specular = specularAmount * light.specular * materialSpecular * shadowFactor;
    return ((ambient + diffuse) * diffuseMapColor + specular * specularMapColor) * atten;
}

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    if (useBumpMap)
    {
        float bumpMapColor = bumpMap.SampleLevel(textureSampler, input.texCoord, 0.0f).r; //  - 0.5f 
        localPosition += (input.normal * bumpMapColor * bumpWeight);
    }
    
    matrix toNDC = wvp[0];
    matrix toLightNDC = wvp[1];
    matrix toWorld = world;
    
    if (useSkinning)
    {
        matrix boneTransform = GetBoneTransform(input.blendIndices, input.blendWeights);
        toNDC = mul(boneTransform, toNDC);
        toLightNDC = mul(boneTransform, toLightNDC);
        toWorld = mul(boneTransform, toWorld);
    }
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), toNDC);
    output.worldPosition = mul(float4(localPosition, 1.0f), toWorld).xyz;
    output.worldNormal = mul(input.normal, (float3x3) toWorld);
    output.worldTangent = mul(input.tangent, (float3x3) toWorld);
    output.dirToLight = -lightDirection;
    output.dirToView = normalize(viewPosition - output.worldPosition);
    output.texCoord = (input.texCoord * tiling) + tilingOffset;
    output.lightNDCPosition = mul(float4(localPosition, 1.0f), toLightNDC);
    output.fogFactor = saturate((fogEnd - output.position.w) / (fogEnd - fogStart));
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 n = normalize(input.worldNormal);
    float3 t = normalize(input.worldTangent);
    float3 b = normalize(cross(n, t));
    
    float3 L = normalize(input.dirToLight);
    float3 V = normalize(input.dirToView);
    
    if (useNormalMap)
    {
        float3x3 tbnw = float3x3(t, b, n);
        float4 normalMapColor = normalMap.Sample(textureSampler, input.texCoord);
        float3 unpackedNormal = normalize(float3((normalMapColor.xy * 2.0f) - 1.0f, normalMapColor.z));
        n = mul(unpackedNormal, tbnw);
    }
    
    float4 ambient = lightAmbient * materialAmbient;
    
    float d = saturate(dot(L, n)); //Saturate(v) === max(v, 0)
    float4 diffuse = d * lightDiffuse * materialDiffuse;
    
    float3 r = reflect(-L, n);
    float base = saturate(dot(r, V));
    float s = pow(base, materialPower);
    float4 specular = s * lightSpecular * materialSpecular;
    
    float4 diffuseMapColor = useDiffuseMap ? diffuseMap.Sample(textureSampler, input.texCoord) : 1.0f;
    float specularMapColor = useSpecularMap ? specularMap.Sample(textureSampler, input.texCoord).r : 1.0f;

    float shadowFactor = 1.0f;
    if (useShadowMap)
    {
        shadowFactor = ComputeShadowFactorPCSS(shadowMap, input.lightNDCPosition, depthBias, d, input.position.xy);
    }

    // Shadow scales diffuse and specular smoothly (0 = fully shadowed, 1 = lit)
    float4 finalColor = (ambient + diffuse * shadowFactor + materialEmissive) * diffuseMapColor
                      + (specular * specularMapColor * shadowFactor);

    for (int spotLightIndex = 0; spotLightIndex < spotLightCount; ++spotLightIndex)
    {
        finalColor += ComputeSpotLightContribution(spotLightIndex, input.worldPosition, n, V,
                                                   diffuseMapColor, specularMapColor, input.position.xy);
    }
    
    if (useFog)
    {
        finalColor = input.fogFactor * finalColor + (1.0f - input.fogFactor) * fogColor;
    }
    
    //frac(sin(dot(u * v, float2(12.9898f, 78.233f))) * 43758.5453123f);
    return finalColor;
}
