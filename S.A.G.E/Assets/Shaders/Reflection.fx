// Description: Reflection shader for SAGE

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp[2];
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

cbuffer MaterialBuffer : register(b2)
{
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float4 materialEmissive;
    float  materialPower;
}

cbuffer ReflectionBuffer : register(b3)
{
    matrix reflectionMatrix;
}

Texture2D diffuseMap : register(t0);
Texture2D reflectionTexture : register(t1);
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
    float4 position             : SV_Position;
    float3 worldNormal          : NORMAL;
	float3 worldTangent         : TANGENT;
    float3 dirToLight           : TEXCOORD0;
    float3 dirToView            : TEXCOORD1;
    float2 texCoord             : TEXCOORD2;
    float4 lightNDCPosition     : TEXCOORD3;
    float4 reflectionPosition   : TEXCOORD4;
};

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    
    const matrix toNDC = wvp[0];
    const matrix toLightNDC = wvp[1];
    const matrix toWorld = world;
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), toNDC);
    output.worldNormal = mul(input.normal, (float3x3) toWorld);
    output.worldTangent = mul(input.tangent, (float3x3) toWorld);
    output.dirToLight = -lightDirection;
    output.dirToView = normalize(viewPosition - mul(float4(localPosition, 1.0f), toWorld).xyz);
    output.texCoord = input.texCoord;
    output.lightNDCPosition = mul(float4(localPosition, 1.0f), toLightNDC);
    
    // Create the reflection projection world matrix.
    matrix reflectProjectWorld;
    reflectProjectWorld = mul(reflectionMatrix, toNDC);
    reflectProjectWorld = mul(toWorld, reflectProjectWorld);
    
    // Calculate the input position against the reflectProjectWorld matrix.
    output.reflectionPosition = mul(output.position, reflectProjectWorld);
    
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
    
    float4 diffuseMapColor = diffuseMap.Sample(textureSampler, input.texCoord);
    float4 finalColor = (ambient + diffuse + materialEmissive) * diffuseMapColor + specular;
    
    return finalColor;
}