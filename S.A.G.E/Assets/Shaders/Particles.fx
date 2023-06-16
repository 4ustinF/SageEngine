// Description: Particle shader for SAGE

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp;
    float3 viewPosition;
    float padding; 
}

cbuffer SettingBuffer : register(b1)
{
	float4 particleColor;
	float2 uvOffSet;
}

Texture2D diffuseMap : register(t0);
SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position     : SV_Position;
    float3 dirToView    : TEXCOORD0;
    float2 texCoord     : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    
    matrix toNDC = wvp;
    matrix toWorld = world;
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), toNDC);
    output.dirToView = normalize(viewPosition - mul(float4(localPosition, 1.0f), toWorld).xyz);
    output.texCoord = input.texCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{        
	float4 finalColor = diffuseMap.Sample(textureSampler, input.texCoord + uvOffSet);
	finalColor *= particleColor;
    return finalColor;
}