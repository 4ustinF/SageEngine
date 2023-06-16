// Description: Toon shader for SAGE

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp;
    float3 viewPosition;
    float bumpWeight; //Padding which we can use for the Bump Multiplyer
}

cbuffer LightBuffer : register(b1)
{
    float3 lightDirection;
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
}

cbuffer SettingBuffer : register(b2)
{
    bool useDiffuseMap;
    bool useCelMap;
    bool useRim;
    float rimIntensity;
}

Texture2D diffuseMap : register(t0);
Texture2D celMap : register(t1);
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
    float4 position     : SV_Position;
    float3 worldNormal  : NORMAL;
	float3 worldTangent : TANGENT;
    float3 dirToLight   : TEXCOORD0;
    float3 dirToView    : TEXCOORD1;
    float2 texCoord     : TEXCOORD2;
};

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3) world);
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.dirToLight = -lightDirection;
    output.dirToView = normalize(viewPosition - mul(float4(localPosition, 1.0f), world).xyz);
    output.texCoord = input.texCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 n = normalize(input.worldNormal);
    float3 t = normalize(input.worldTangent);
	float3 b = normalize(cross(n, t));
    
    float3 L = normalize(input.dirToLight);
    float3 V = normalize(input.dirToView);
    
    float d = saturate(dot(L, n));
    
    if (!useCelMap) {
        if (d <= 0.0f) {
            d = 0.0f;
        }
        else if (d <= 0.2f) {
            d = 0.2f;
        }
        else if (d <= 0.6f) {
            d = 0.6f;
        }
        else if (d <= 0.85f) {
            d = 0.85f;
        }
        else if (d <= 1.0f) {
            d = 1.0f;
        }
    }

    float4 diffuse = d * lightDiffuse;
    
    float rim = 1.0f - clamp(dot(input.dirToView, input.worldNormal), 0.0f, 1.0f);
    float4 RimColor = float4(0.0f, 0.0f, 0.0f, 1.0f) * pow(rim, 1.0f);
    
    float4 diffuseMapColor = useDiffuseMap ? diffuseMap.Sample(textureSampler, input.texCoord) : 1.0f;
    float4 finalColor = (lightAmbient + diffuse) * diffuseMapColor;
    
    float4 color = finalColor + RimColor;
    
    if (useRim && rim > rimIntensity) {
        return RimColor;
    }
    else if (useCelMap) {
        float4 celColor = celMap.Sample(textureSampler, float2(d, 0.0f));
        return finalColor + finalColor * celColor;
    }
    else {
        return finalColor;
    }
}