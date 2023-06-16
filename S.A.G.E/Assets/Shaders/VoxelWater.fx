// Description: Shader for voxel water

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp[2];
    float3 viewPosition;
    float chunkSize;
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
    float materialPower;
}

cbuffer SettingBuffer : register(b3)
{
    float deltaTime;
    float amplitude;
    float frequency;
    float waveLength;
    bool useFog;
    float fogStart;
    float fogEnd;
    float4 fogColor;
}

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D normalMap : register(t2);

SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texCoord : TEXCOORD;
    int4 blendIndices : BLENDINDICES;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float3 worldNormal : NORMAL;
    float3 worldTangent : TANGENT;
    float3 dirToLight : TEXCOORD0;
    float3 dirToView : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
    float fogFactor : FOG;
};

float3 GetWavePosition(float3 localPosition)
{
    float posX = localPosition.x;
    float posZ = localPosition.z;
    
    const float halfChunkSize = chunkSize * 0.5f;
    while (posX > halfChunkSize)
    {
        posX -= chunkSize;
    }
    while (posX < -halfChunkSize)
    {
        posX += chunkSize;
    }
    
    while (posZ > halfChunkSize)
    {
        posZ -= chunkSize;
    }
    while (posZ < -halfChunkSize)
    {
        posZ += chunkSize;
    }
    
    localPosition.y += amplitude * sin(posX * posZ * frequency);
    //float k = 2.0f * 3.14159265f * waveLength;
    //localPosition.y += amplitude * sin(k * posX * posZ * frequency);
    
    return localPosition;
}

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    if (input.blendIndices[0] == 1)
    {
        //localPosition = GetWavePosition(localPosition);
    }
    
    matrix toNDC = wvp[0];
    matrix toLightNDC = wvp[1];
    matrix toWorld = world;
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), toNDC);
    output.worldNormal = mul(input.normal, (float3x3) toWorld);
    output.worldTangent = mul(input.tangent, (float3x3) toWorld);
    output.dirToLight = -lightDirection;
    output.dirToView = normalize(viewPosition - mul(float4(localPosition, 1.0f), toWorld).xyz);
    output.texCoord = input.texCoord;
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
    
    float3x3 tbnw = float3x3(t, b, n);
    float4 normalMapColor = normalMap.Sample(textureSampler, input.texCoord);
    float3 unpackedNormal = normalize(float3((normalMapColor.xy * 2.0f) - 1.0f, normalMapColor.z));
    n = mul(unpackedNormal, tbnw);
    
    float4 ambient = lightAmbient * materialAmbient;
    
    float d = clamp(dot(L, n), 0.4f, 0.6f);
    float4 diffuse = d * lightDiffuse * materialDiffuse;
    
    float3 r = reflect(-L, n);
    float base = clamp(dot(r, V), 0.4f, 0.75f);
    float s = pow(base, materialPower);
    float4 specular = s * lightSpecular * materialSpecular;
    
    float4 diffuseMapColor = diffuseMap.Sample(textureSampler, input.texCoord);
    float specularMapColor = specularMap.Sample(textureSampler, input.texCoord).r;
    float4 finalColor = (ambient + diffuse + materialEmissive) * diffuseMapColor + (specular * specularMapColor);
    
    if (useFog)
    {
        finalColor = input.fogFactor * finalColor + (1.0f - input.fogFactor) * fogColor;
    }
    
    return finalColor;
}