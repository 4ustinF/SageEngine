// Description: Cross hatch shader for SAGE

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

cbuffer SettingBuffer : register(b2)
{
    bool useDiffuseMap;
    bool useSpecularMap;
    bool useNormalMap;
    float param0; // Intensity multiplier
}

cbuffer MaterialBuffer : register(b3)
{
    float4 materialAmbient;
    float4 materialDiffuse;
    float4 materialSpecular;
    float4 materialEmissive;
    float materialPower;
}

Texture2D hatchMap0 : register(t0);
Texture2D hatchMap1 : register(t1);
Texture2D diffuseMap : register(t2);
Texture2D specularMap : register(t3);
Texture2D normalMap : register(t4);

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
    float4 lightNDCPosition : TEXCOORD3;
};

float3 Hatching(float2 _uv, half _intensity)
{
    half3 hatch0 = hatchMap0.Sample(textureSampler, _uv).rgb;
    half3 hatch1 = hatchMap1.Sample(textureSampler, _uv).rgb;

    half3 overbright = max(0.0f, _intensity - 1.0f);

    half3 weightsA = saturate((_intensity * 6.0f) + half3(-0, -1, -2));
    half3 weightsB = saturate((_intensity * 6.0f) + half3(-3, -4, -5));

    weightsA.xy -= weightsA.yz;
    weightsA.z -= weightsB.x;
    weightsB.xy -= weightsB.yz;

    hatch0 = hatch0 * weightsA;
    hatch1 = hatch1 * weightsB;

    half3 hatching = overbright + hatch0.r +
								 hatch0.g + hatch0.b +
								 hatch1.r + hatch1.g +
								 hatch1.b;

    return hatching;
}

VS_OUTPUT VS(VS_INPUT input)
{
    matrix toNDC = wvp[0];
    matrix toLightNDC = wvp[1];
    matrix toWorld = world;
    
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), toNDC);
    output.worldNormal = mul(input.normal, (float3x3) toWorld);
    output.worldTangent = mul(input.tangent, (float3x3) toWorld);
    output.dirToLight = -lightDirection;
    output.dirToView = normalize(viewPosition - mul(float4(input.position, 1.0f), toWorld).xyz);
    output.texCoord = input.texCoord;
    output.lightNDCPosition = mul(float4(input.position, 1.0f), toLightNDC);
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
    
    float d = saturate(dot(L, n));
    float4 diffuse = d * lightDiffuse * materialDiffuse;
    
    float3 r = reflect(-L, n);
    float base = saturate(dot(r, V));
    float s = pow(base, materialPower);
    float4 specular = s * lightSpecular * materialSpecular;
    
    float specularMapColor = useSpecularMap ? specularMap.Sample(textureSampler, input.texCoord).r : 1.0f;
    float4 diffuseMapColor = useDiffuseMap ? diffuseMap.Sample(textureSampler, input.texCoord) : 1.0f;
    float4 finalColor = (ambient + diffuse + materialEmissive) * diffuseMapColor + (specular * specularMapColor);
    
    half intensity = dot(finalColor.rgb, half3(0.2326, 0.7152, 0.0722));
    
    finalColor.rgb = Hatching(input.texCoord * 8, intensity * param0);
    
    return finalColor;
}