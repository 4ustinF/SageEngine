// Description: Standard shader for SAGE

#define MAX_SPOT_LIGHTS 4

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
    float  materialPower;
}

cbuffer SettingBuffer : register(b4)
{
    bool useDiffuseMap;
    bool useSpecularMap;
    bool useBumpMap;
    bool useNormalMap;
    bool useShadowMap;
    bool useSkinning;
    float depthBias;
    int sampleSize;
    
    float4 fogColor;
    bool useFog;
    float fogStart;
    float fogEnd;
    bool useSpotShadows;

    float2 tiling;
    float2 tilingOffset;
}

cbuffer SpotLightBuffer : register(b5)
{
    SpotLightData spotLights[MAX_SPOT_LIGHTS];
    int spotLightCount;
    float3 spotLightPadding;
}

cbuffer SpotLightMatrixBuffer : register(b6)
{
    matrix spotLightViewProj[MAX_SPOT_LIGHTS];
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
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 texCoord : TEXCOORD;
    int4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHT;
};

struct VS_OUTPUT
{
    float4 position     : SV_Position;
    float3 worldPosition : TEXCOORD4;
    float3 worldNormal  : NORMAL;
	float3 worldTangent : TANGENT;
    float3 dirToLight   : TEXCOORD0;
    float3 dirToView    : TEXCOORD1;
    float2 texCoord     : TEXCOORD2;
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

matrix GetBoneTransform(int4 indices, float4 weights)
{
    if (length(weights) <= 0.0f) {
        return Identity;
    }
    
    matrix transform;
    transform = boneTransforms[indices[0]] * weights[0];
    transform += boneTransforms[indices[1]] * weights[1];
    transform += boneTransforms[indices[2]] * weights[2];
    transform += boneTransforms[indices[3]] * weights[3];
    return transform;
}

float ComputeShadowFactor(Texture2D shadowTex, float4 lightNDCPosition, float bias, int sampleSize)
{
    float actualDepth = 1.0f - (lightNDCPosition.z / lightNDCPosition.w);
    float2 shadowUV = lightNDCPosition.xy / lightNDCPosition.w;
    float u = (shadowUV.x + 1.0f) * 0.5f;
    float v = 1.0f - (shadowUV.y + 1.0f) * 0.5f;

    if (saturate(u) != u || saturate(v) != v)
        return 1.0f; // outside frustum: fully lit

    float savedDepth = shadowTex.Sample(textureSampler, float2(u, v)).r;
    if (savedDepth <= actualDepth + bias)
        return 1.0f;

    float shadowMult = 0.0f;
    int width, height;
    shadowTex.GetDimensions(width, height);
    float2 texelSize = 1.0 / float2(width, height);
    int size = clamp(sampleSize, 0, 5);
    for (int x = -size; x <= size; ++x)
        for (int y = -size; y <= size; ++y)
            shadowMult += savedDepth > shadowTex.Sample(textureSampler, float2(u + x * texelSize.x, v + y * texelSize.y)).r + bias ? 1.0f : 0.0f;

    int amt = size * 2 + 1;
    return shadowMult / (amt * amt);
}

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    if (useBumpMap) {
        float bumpMapColor = bumpMap.SampleLevel(textureSampler, input.texCoord, 0.0f).r; //  - 0.5f 
        localPosition += (input.normal * bumpMapColor * bumpWeight);
    }
    
    matrix toNDC = wvp[0];
    matrix toLightNDC = wvp[1];
    matrix toWorld = world;
    
    if (useSkinning) {
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
    
    if (useNormalMap) {
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
    float4 finalColor = (ambient + diffuse + materialEmissive) * diffuseMapColor + (specular * specularMapColor);
    
    if (useShadowMap) {
        float actualDepth = 1.0f - (input.lightNDCPosition.z / input.lightNDCPosition.w);
        float2 shadowUV = input.lightNDCPosition.xy / input.lightNDCPosition.w;
        float u = (shadowUV.x + 1.0f) * 0.5f;
        float v = 1.0f - (shadowUV.y + 1.0f) * 0.5f;

        if (saturate(u) == u && saturate(v) == v)
        {
            float4 savedColor = shadowMap.Sample(textureSampler, float2(u, v));
            float savedDepth = savedColor.r;
            if (savedDepth > actualDepth + depthBias)
            {
                float shadowMult = 0.0f;
                int width, height;
                shadowMap.GetDimensions(width, height);
                float2 texelSize = 1.0 / float2(width, height);
                
                //sampleSize
                int size = 1;
                if (sampleSize <= 0) { size = 0; }
                else if (sampleSize == 2) { size = 2; }
                else if (sampleSize == 3) { size = 3; }
                else if (sampleSize == 4) { size = 4; }
                else if (sampleSize >= 5) { size = 5; }
                
                for (int x = -size; x <= size; ++x) {
                    for (int y = -size; y <= size; ++y) {
                        float pcfDepth = shadowMap.SampleLevel(textureSampler, float2(u + x * texelSize.x, v + y * texelSize.y), 0).r;
                        shadowMult += savedDepth > pcfDepth + depthBias ? 1.0f : 0.0f;
                    }
                }
                int amt = size * 2 + 1;
                shadowMult /= amt * amt;
                //shadowMult = 0.0f;
                
                float4 trans = diffuse * shadowMult; // shadowMult = 0.0f(black) - 1.0f(clear)
                finalColor = (ambient + materialEmissive + trans) * diffuseMapColor;
            }
        }
    }
    
#define SPOT_LIGHT_CONTRIBUTION(IDX) \
    if (IDX < spotLightCount) \
    { \
        SpotLightData light = spotLights[IDX]; \
        float3 toLight = light.position - input.worldPosition; \
        float dist = length(toLight); \
        float3 spotL = toLight / dist; \
        float cosAngle = dot(-spotL, normalize(light.direction)); \
        float spotFactor = smoothstep(cos(light.outerConeAngle), cos(light.innerConeAngle), cosAngle); \
        if (spotFactor > 0.0f) \
        { \
            float atten = spotFactor / max(light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist, 0.0001f); \
            float sd = saturate(dot(spotL, n)); \
            float3 sr = reflect(-spotL, n); \
            float ss = pow(saturate(dot(sr, V)), materialPower); \
            float shadowFactor = 1.0f; \
            if (useSpotShadows) \
            { \
                float4 spotNDC = mul(float4(input.worldPosition, 1.0f), spotLightViewProj[IDX]); \
                shadowFactor = ComputeShadowFactor(spotShadowMaps[IDX], spotNDC, depthBias, sampleSize); \
            } \
            float4 spotAmb  = light.ambient * materialAmbient; \
            float4 spotDiff = sd * light.diffuse * materialDiffuse * shadowFactor; \
            float4 spotSpec = ss * light.specular * materialSpecular * shadowFactor; \
            finalColor += ((spotAmb + spotDiff) * diffuseMapColor + spotSpec * specularMapColor) * atten; \
        } \
    }

SPOT_LIGHT_CONTRIBUTION(0)
SPOT_LIGHT_CONTRIBUTION(1)
SPOT_LIGHT_CONTRIBUTION(2)
SPOT_LIGHT_CONTRIBUTION(3)
    
    if (useFog)
    {
        finalColor = input.fogFactor * finalColor + (1.0f - input.fogFactor) * fogColor;
    }
    
    //frac(sin(dot(u * v, float2(12.9898f, 78.233f))) * 43758.5453123f);
    return finalColor;
}