// Description: Standard shader for SAGE

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

cbuffer MaterialBuffer : register(b2)
{
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float  materialPower;
}

cbuffer SettingBuffer : register(b3)
{
    bool useDiffuseMap;
    bool useSpecularMap;
    bool useBumpMap;
    bool useNormalMap;
}

Texture2D diffueMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D bumpMap : register(t2);
Texture2D normalMap : register(t3);

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
    float bumpStrength = useBumpMap ? bumpMap.SampleLevel(textureSampler, input.texCoord, 0.0f).r - 0.5f : 0.0f;
    
    float3 localPosition = input.position;
    localPosition += (input.normal * bumpStrength * bumpWeight);
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3)world);
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
    
    if (useNormalMap) {
        float3x3 tbnw = float3x3(t, b, n);
        float4 normalMapColor = normalMap.Sample(textureSampler, input.texCoord);
        float3 unpackedNormal = normalize(float3((normalMapColor.xy * 2.0f) - 1.0f, normalMapColor.z));
        n = mul(unpackedNormal, tbnw);
    }
    
    float4 ambient = lightAmbient * materialAmbient;
    
    float d = saturate(dot(L, n)); //Saturate(v) === max(v, 0)
    float4 diffuse = useDiffuseMap ? d * lightDiffuse * materialDiffuse : 0.0f;
    
    float3 r = reflect(-L, n);
    float base = saturate(dot(r, V));
    float s = pow(base, materialPower);
    float4 specular = s * lightSpecular * materialSpecular;
    
    float4 diffuseColorMap = diffueMap.Sample(textureSampler, input.texCoord);
    float specularColorMap = useSpecularMap ? specularMap.Sample(textureSampler, input.texCoord).r : 0.0f;
    float4 finalColor = (ambient + diffuse) * diffuseColorMap + (specular * specularColorMap);
    return finalColor;
}