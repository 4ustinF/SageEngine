// Description: Voxel Terrain shader for SAGE that supports greedy meshing

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp[2];
    float3 viewPosition;
    float padding;
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
    bool useShadowMap;
    float depthBias;
    bool useFog;
    float fogStart;
    
    float fogEnd;
    
    float4 fogColor;
}

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D bumpMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D shadowMap : register(t4);

Texture2D bedRockTexture : register(t5);
Texture2D grassTopTexture : register(t6);
Texture2D grassSideTexture : register(t7);
Texture2D dirtTexture : register(t8);
Texture2D cobbleStoneTexture : register(t9);
Texture2D stoneTexture : register(t10);
Texture2D oakLeafsTexture : register(t11);
Texture2D oakLogTopTexture : register(t12);
Texture2D oakLogSideTexture : register(t13);
Texture2D oakPlankTexture : register(t14);
Texture2D glassTexture : register(t15);
Texture2D sandTexture : register(t16);
Texture2D gravelTexture : register(t17);
Texture2D coalOreTexture : register(t18);
Texture2D ironOreTexture : register(t19);
Texture2D goldOreTexture : register(t20);
Texture2D diamondOreTexture : register(t21);
Texture2D redStoneOreTexture : register(t22);
Texture2D lapisOreTexture : register(t23);
Texture2D emeraldOreTexture : register(t24);
Texture2D snowGrassSideOreTexture : register(t25);
Texture2D snowTexture : register(t26);
Texture2D cactusTopTexture : register(t27);
Texture2D cactusSideTexture : register(t28);
Texture2D grassTexture : register(t29);
Texture2D poppyTexture : register(t30);
Texture2D dandelionTexture : register(t31);
Texture2D sugarCaneTexture : register(t32);
Texture2D craftingTopTexture : register(t33);
Texture2D craftingFrontSideTexture : register(t34);
Texture2D craftingSideTexture : register(t35);
Texture2D furnaceTopTexture : register(t36);
Texture2D furnaceFrontSideTexture : register(t37);
Texture2D furnaceSideTexture : register(t38);
Texture2D farmlandTexture : register(t39);
Texture2D wheat0Texture : register(t40);
Texture2D wheat1Texture : register(t41);
Texture2D wheat2Texture : register(t42);
Texture2D wheat3Texture : register(t43);
Texture2D wheat4Texture : register(t44);
Texture2D wheat5Texture : register(t45);
Texture2D wheat6Texture : register(t46);
Texture2D wheat7Texture : register(t47);

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
    float4 lightNDCPosition : TEXCOORD3;
    int textureIndex : TEXCOORD4;
    float fogFactor : FOG;
};

float4 DiffuseSampleTexture(int index, float2 texCoord)
{
    switch (index)
    {
        case 0: return bedRockTexture.Sample(textureSampler, texCoord);
        case 1: return grassTopTexture.Sample(textureSampler, texCoord);
        case 2: return grassSideTexture.Sample(textureSampler, texCoord);
        case 3: return dirtTexture.Sample(textureSampler, texCoord);
        case 4: return cobbleStoneTexture.Sample(textureSampler, texCoord);
        case 5: return stoneTexture.Sample(textureSampler, texCoord);
        case 6: return oakLeafsTexture.Sample(textureSampler, texCoord);
        case 7: return oakLogTopTexture.Sample(textureSampler, texCoord);
        case 8: return oakLogSideTexture.Sample(textureSampler, texCoord);
        case 9: return oakPlankTexture.Sample(textureSampler, texCoord);
        case 10: return glassTexture.Sample(textureSampler, texCoord);
        case 11: return sandTexture.Sample(textureSampler, texCoord);
        case 12: return gravelTexture.Sample(textureSampler, texCoord);
        case 13: return coalOreTexture.Sample(textureSampler, texCoord);
        case 14: return ironOreTexture.Sample(textureSampler, texCoord);
        case 15: return goldOreTexture.Sample(textureSampler, texCoord);
        case 16: return diamondOreTexture.Sample(textureSampler, texCoord);
        case 17: return redStoneOreTexture.Sample(textureSampler, texCoord);
        case 18: return lapisOreTexture.Sample(textureSampler, texCoord);
        case 19: return emeraldOreTexture.Sample(textureSampler, texCoord);
        case 20: return snowGrassSideOreTexture.Sample(textureSampler, texCoord);
        case 21: return snowTexture.Sample(textureSampler, texCoord);
        case 22: return cactusTopTexture.Sample(textureSampler, texCoord);
        case 23: return cactusSideTexture.Sample(textureSampler, texCoord);
        case 24: return grassTexture.Sample(textureSampler, texCoord);
        case 25: return poppyTexture.Sample(textureSampler, texCoord);
        case 26: return dandelionTexture.Sample(textureSampler, texCoord);
        case 27: return sugarCaneTexture.Sample(textureSampler, texCoord);
        case 28: return craftingTopTexture.Sample(textureSampler, texCoord);
        case 29: return craftingFrontSideTexture.Sample(textureSampler, texCoord);
        case 30: return craftingSideTexture.Sample(textureSampler, texCoord);
        case 31: return furnaceTopTexture.Sample(textureSampler, texCoord);
        case 32: return furnaceFrontSideTexture.Sample(textureSampler, texCoord);
        case 33: return furnaceSideTexture.Sample(textureSampler, texCoord);
        case 34: return farmlandTexture.Sample(textureSampler, texCoord);
        case 35: return wheat0Texture.Sample(textureSampler, texCoord);
        case 36: return wheat1Texture.Sample(textureSampler, texCoord);
        case 37: return wheat2Texture.Sample(textureSampler, texCoord);
        case 38: return wheat3Texture.Sample(textureSampler, texCoord);
        case 39: return wheat4Texture.Sample(textureSampler, texCoord);
        case 40: return wheat5Texture.Sample(textureSampler, texCoord);
        case 41: return wheat6Texture.Sample(textureSampler, texCoord);
        case 42: return wheat7Texture.Sample(textureSampler, texCoord);
        default:
            return grassTopTexture.Sample(textureSampler, texCoord);
    };
}

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    
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
    output.lightNDCPosition = mul(float4(localPosition, 1.0f), toLightNDC);
    output.textureIndex = input.blendIndices[0];
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
    
    float4 ambient = lightAmbient * materialAmbient;
    
    float d = saturate(dot(L, n));
    float4 diffuse = d * lightDiffuse * materialDiffuse;
    
    float3 r = reflect(-L, n);
    float base = saturate(dot(r, V));
    float s = pow(base, materialPower);
    float4 specular = s * lightSpecular * materialSpecular;
    
    float4 diffuseMapColor = DiffuseSampleTexture(input.textureIndex, input.texCoord);
    
    float4 finalColor = (ambient + diffuse + materialEmissive) * diffuseMapColor + specular;
    
    if (useShadowMap)
    {
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
                
                float4 trans = diffuse * shadowMult; // shadowMult = 0.0f(black) - 1.0f(clear)
                finalColor = (ambient + materialEmissive + trans) * diffuseMapColor;
            }
        }
    }
    
    if (useFog)
    {
        finalColor = input.fogFactor * finalColor + (1.0f - input.fogFactor) * fogColor;
    }
    
    return finalColor;
}

