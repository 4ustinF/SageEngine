// Description: Texturing shader for SAGE

cbuffer ConstantBuffer : register(b0)
{
    matrix wvp;
}

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D bumpMap : register(t2);
Texture2D normalMap : register(t3);

SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD2;
};


VS_OUTPUT VS(VS_INPUT input)
{    
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.texCoord = input.texCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return diffuseMap.Sample(textureSampler, input.texCoord);
}