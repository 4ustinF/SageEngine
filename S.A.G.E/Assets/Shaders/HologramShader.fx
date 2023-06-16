// Description: Hologram shader for SAGE

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix wvp;
    float3 viewPosition;
    float padding;
}

cbuffer SettingBuffer : register(b1)
{
    bool useDiffuseMap;
    bool useBars;
    bool useAlpha;
    bool shouldFlicker;
    bool hasRim;
    bool hasGlow;
    bool shouldGlitch;
    bool bParam0;
}

cbuffer HologramBuffer : register(b2)
{
    float deltaTime;
    float barSpeed;
    float barDistance;
    float mAlpha;
    float flickerSpeed;
    float rimPower;
    float glowDistance;
    float glowSpeed;
    float4 rimColor; // Color to float4
    float4 mainColor; // Color to float4
    float glitchIntensity;
    float glitchSpeed;
    float param0;
    float param1;
}

Texture2D diffuseMap : register(t0);
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
    float3 dirToView    : TEXCOORD0;
    float2 texCoord     : TEXCOORD1;
};

float rand(float n)
{
    return frac(sin(n) * 43758.5453123f);
}

float noise(float p)
{
    float fl = floor(p);
    float fc = frac(p);
    return lerp(rand(fl), rand(fl + 1.0f), fc);
}

VS_OUTPUT VS(VS_INPUT input)
{
    float3 localPosition = input.position;
    
    VS_OUTPUT output;
    output.position = mul(float4(localPosition, 1.0f), wvp);
    output.worldNormal = mul(input.normal, (float3x3)world);
    output.dirToView = normalize(viewPosition - mul(float4(localPosition, 1.0f), world).xyz);
    output.texCoord = input.texCoord;
    
    if (shouldGlitch) {
        output.position.x += glitchIntensity * step(0.5f, sin(deltaTime * 2.0f + localPosition.y * 1.0f)) * step(0.99f, sin(deltaTime * glitchSpeed * 0.5f));
    }
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 texColor = useDiffuseMap ? diffuseMap.Sample(textureSampler, input.texCoord) : float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float bars = 0.0f;
    if (useBars) {
        float val = deltaTime * barSpeed + input.position.y * barDistance;
        bars = step(val - floor(val), 0.5f) * 0.65f;
    }
    
    float alpha = 1.0f;
    if (useAlpha) {
        alpha = mAlpha;
    }
    
    float flicker = 1.0f;
    if (shouldFlicker) {
        flicker = clamp(noise(deltaTime * flickerSpeed), 0.65f, 1.0f);
    }
    
    float rim = 1.0f;
    float4 RimColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (hasRim) {
        rim = 1.0f - clamp(dot(input.dirToView, input.worldNormal), 0.0f, 1.0f);
        RimColor = rimColor * pow(rim, rimPower);
    }
    
    float glow = 0.0f;
    if (hasGlow) {
        float tempGlow = input.position.y * glowDistance - deltaTime * glowSpeed;
        glow = tempGlow - floor(tempGlow);
    }
    
    float4 color = texColor * mainColor + RimColor + (glow * 0.35f * mainColor);
    color.a = texColor.a * alpha * (bars + rim + glow) * flicker;
    
    return color;
}