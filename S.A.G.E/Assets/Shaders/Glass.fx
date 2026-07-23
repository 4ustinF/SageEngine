// Description: Glass shader for 

#define MAX_GLASS_IMPACTS 3

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp[2];
    float3 viewPosition;
    float bumpWeight;
}

cbuffer LightBuffer : register(b1)
{
    float3 lightDirection;
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
}

cbuffer MaterialBuffer : register(b2)
{
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float4 materialEmissive;
    float  materialPower;
}

cbuffer SettingBuffer : register(b3)
{
    bool useDiffuseMap;
    bool useSpecularMap;
    bool useBumpMap;
    bool useNormalMap;
    
    bool useShadowMap;
    float depthBias;
    int sampleSize;
    float padding0;
    
    float4 fogColor;
    bool useFog;
    float fogStart;
    float fogEnd;
    float padding1;

    float2 tiling;
    float2 tilingOffset;
}

struct GlassImpact
{
    float2 uv;
    float radius;
    float strength;

    float rotation;
    float3 padding;
};

cbuffer GlassBuffer : register(b4)
{
    bool useShatterMap;
    bool useShatterNormalMap;
    float shatterIntensity;
    int impactCount;

    float4 shatterColor;

    GlassImpact impacts[MAX_GLASS_IMPACTS];
}

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D bumpMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D shadowMap : register(t4);
Texture2D shatterMap : register(t5);
Texture2D shatterNormalMap : register(t6);

SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position         : SV_Position;
    float3 worldNormal      : NORMAL;
	float3 worldTangent     : TANGENT;
    float3 dirToLight       : TEXCOORD0;
    float3 dirToView        : TEXCOORD1;
    float2 texCoord         : TEXCOORD2;
    float4 lightNDCPosition : TEXCOORD3;
    float fogFactor         : FOG;
};

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    if (useBumpMap) {
        float bumpMapColor = bumpMap.SampleLevel(textureSampler, input.texCoord, 0.0f).r; //  - 0.5f 
        localPosition += (input.normal * bumpMapColor * bumpWeight);
    }
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), wvp[0]);
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.dirToLight = -lightDirection;
    output.dirToView = normalize(viewPosition - mul(float4(localPosition, 1.0f), world).xyz);
    output.texCoord = (input.texCoord * tiling) + tilingOffset;
    output.lightNDCPosition = mul(float4(localPosition, 1.0f), wvp[1]);
    output.fogFactor = saturate((fogEnd - output.position.w) / (fogEnd - fogStart));
    return output;
}

float ComputeImpactMask(float2 uv, float2 impactUV, float radius)
{
    float dist = distance(uv, impactUV);

    // 1 near impact, 0 outside radius
    float mask = saturate(1.0f - dist / radius);

    // Smooth falloff so it does not look too harsh
    mask = smoothstep(0.0f, 1.0f, mask);

    return mask;
}

float2 ComputeShatterUV(float2 uv, float2 impactUV, float radius)
{
    // Converts mesh UV into local crack texture UV.
    // Impact point becomes center of shatter texture at 0.5, 0.5.
    return ((uv - impactUV) / radius) * 0.5f + 0.5f;
}

float2 RotateUV(float2 uv, float angle)
{
    float s = sin(angle);
    float c = cos(angle);

    uv -= 0.5f;

    float2 rotated;
    rotated.x = uv.x * c - uv.y * s;
    rotated.y = uv.x * s + uv.y * c;

    return rotated + 0.5f;
}

bool IsInsideUV(float2 uv)
{
    return uv.x >= 0.0f && uv.x <= 1.0f && uv.y >= 0.0f && uv.y <= 1.0f;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 n = normalize(input.worldNormal);
    float3 t = normalize(input.worldTangent);
	float3 b = normalize(cross(n, t));
    float3x3 tbnw = float3x3(t, b, n);
    
    float3 L = normalize(input.dirToLight);
    float3 V = normalize(input.dirToView);
    
    if (useNormalMap) 
    {
        float4 normalMapColor = normalMap.Sample(textureSampler, input.texCoord);
        float3 unpackedNormal = normalize(float3((normalMapColor.xy * 2.0f) - 1.0f, normalMapColor.z));
        n = mul(unpackedNormal, tbnw);
    }
    
    float finalShatterMask = 0.0f;
    float3 finalShatterNormal = float3(0.0f, 0.0f, 1.0f);

    // Impact cracks
    if (useShatterMap || useShatterNormalMap)
    {
        for (int i = 0; i < impactCount && i < MAX_GLASS_IMPACTS; ++i)
        {
            float2 impactUV = impacts[i].uv;
            float radius = impacts[i].radius;
            float strength = impacts[i].strength;
            float rotation = impacts[i].rotation;

            float impactMask =
        ComputeImpactMask(input.texCoord, impactUV, radius)
        * strength;

            if (impactMask > 0.001f)
            {
                float2 shatterUV =
            ComputeShatterUV(input.texCoord, impactUV, radius);

                shatterUV = RotateUV(shatterUV, rotation);

                if (IsInsideUV(shatterUV))
                {
                    float textureMask = 1.0f;

                    if (useShatterMap)
                    {
                        textureMask =
                    shatterMap.Sample(textureSampler, shatterUV).r;
                    }

                    float crackAmount =
                impactMask * textureMask;

                    finalShatterMask =
                max(finalShatterMask, crackAmount);

                    if (useShatterNormalMap)
                    {
                        float3 shatterNormalColor =
                    shatterNormalMap.Sample(
                        textureSampler,
                        shatterUV).xyz;

                        float3 unpackedShatterNormal =
                    normalize(float3(
                        shatterNormalColor.xy * 2.0f - 1.0f,
                        shatterNormalColor.z));

                        finalShatterNormal =
                    lerp(
                        finalShatterNormal,
                        unpackedShatterNormal,
                        crackAmount);
                    }
                }
            }
        }

    }

    // Blend crack normal into current normal
    if (useShatterNormalMap)
    {
        float3 worldShatterNormal = normalize(mul(finalShatterNormal, tbnw));
        n = normalize(lerp(n, worldShatterNormal, finalShatterMask * shatterIntensity));
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
    
    // Add visible white-ish crack lines
    if (useShatterMap)
    {
        float crackBlend = saturate(finalShatterMask * shatterIntensity);
        finalColor.rgb = lerp(finalColor.rgb, shatterColor.rgb, crackBlend * shatterColor.a);
    }
    
    if (useShadowMap)
    {
        float actualDepth = 1.0f - (input.lightNDCPosition.z / input.lightNDCPosition.w);
        float2 shadowUV = input.lightNDCPosition.xy / input.lightNDCPosition.w;
        float u = (shadowUV.x + 1.0f) * 0.5f;
        float v = 1.0f - (shadowUV.y + 1.0f) * 0.5f;

        if (saturate(u) == u && saturate(v) == v)
        {
            float savedDepth = shadowMap.Sample(textureSampler, float2(u, v)).r;

            if (savedDepth > actualDepth + depthBias)
            {
                finalColor = (ambient + materialEmissive) * diffuseMapColor;
            }
        }
    }
    
    if (useFog)
    {
        finalColor = input.fogFactor * finalColor + (1.0f - input.fogFactor) * fogColor;
    }
    
    return finalColor;
}
