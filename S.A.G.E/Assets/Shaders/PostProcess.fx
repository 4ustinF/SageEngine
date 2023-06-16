// Description: Post Processing shader for SAGE

cbuffer PostProcessBuffer : register(b0)
{
    int mode;
    float param0;
    float param1;
    float param2;
}

Texture2D textureMap0 : register(t0);
Texture2D textureMap1 : register(t1);
Texture2D textureMap2 : register(t2);
Texture2D textureMap3 : register(t3);
SamplerState textureSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 finalColor;
	
    if (mode == 0) // None
    {
        finalColor = textureMap0.Sample(textureSampler, input.texCoord);
    }
    else if (mode == 1) // Monochrome
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        finalColor = (color.r + color.g + color.b) / 3.0f;
    }
    else if (mode == 2) // Invert
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        finalColor = 1.0f - color;
    }
    else if (mode == 3) // Mirror
    {
        float2 texCoord = input.texCoord;
        texCoord.x = 1 - texCoord.x;
        finalColor = textureMap0.Sample(textureSampler, texCoord);
    }
    else if (mode == 4) // Grain
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        
        float intensity = param1;
        
        float x = (input.texCoord.x + 4.0f) * (input.texCoord.y + 4.0f) * (param0 * 10.0f);
        
        float grainAmt = fmod((fmod(x, 13.0f) + 1.0f) * (fmod(x, 123.0f) + 1.0f), 0.01f) - 0.005f;
        float4 grain = float4(grainAmt, grainAmt, grainAmt, grainAmt) * intensity;
        
        finalColor = color + grain;
    }
    else if (mode == 5) // ObjectiveGrain
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        
        float intensity = param1;
        
        float x = (input.texCoord.x + 4.0f) * (input.texCoord.y + 4.0f) * (param0 * 10.0f);
        
        float grainAmt = fmod((fmod(x, 13.0f) + 1.0f) * (fmod(x, 123.0f) + 1.0f), 0.01f) - 0.005f;
        float4 grain = float4(grainAmt, grainAmt, grainAmt, grainAmt) * intensity;
        
        grain = 1.0f - grain;
        finalColor = color * grain;
    }
    else if (mode == 6) // Vignette 
    {
        float4 color = textureMap0.Sample(textureSampler, input.texCoord);
        
        float2 uv = input.texCoord;
        
        uv *= 1.0f - uv.yx;
        
        float vig = uv.x * uv.y * param0; // multiply with param0 for intensity
        vig = pow(vig, -param1); // param1 changes pow for modifying the extend of the vignette
        
        float4 shade = float4(vig, vig, vig, vig);

        finalColor = lerp(color, -shade, vig);
    }
    else if (mode == 7) // Sinethresholder
    {
        float2 uv = input.texCoord;
        float r = textureMap0.Sample(textureSampler, input.texCoord).r;
        float c = step(0.0f, sin(uv.x * param1 + r * param0));
        
        finalColor = float4(c, c, c, 1.0f);
    }
    else if (mode == 8) // Blur1
    {
        finalColor = textureMap0.Sample(textureSampler, input.texCoord.xy) * param0; // 0.6f - Brightness
        
        int loopAmt = param1;
        for (int i = 0; i < loopAmt; ++i) // 3 - Times to loop
        {
            finalColor += textureMap0.Sample(textureSampler, input.texCoord.xy + (param2)) * .2f; // 0.005f is offset amount
            finalColor += textureMap0.Sample(textureSampler, input.texCoord.xy - (param2)) * .2f;
        }
    }
    else if (mode == 9) // Blur2
    {
        float u = input.texCoord.x;
        float v = input.texCoord.y;
        
        finalColor =
        textureMap0.Sample(textureSampler, float2(u, v))
        + textureMap0.Sample(textureSampler, float2(u + param0, v))
        + textureMap0.Sample(textureSampler, float2(u - param0, v))
        + textureMap0.Sample(textureSampler, float2(u, v + param1))
        + textureMap0.Sample(textureSampler, float2(u, v - param1))
        + textureMap0.Sample(textureSampler, float2(u + param0, v + param1))
        + textureMap0.Sample(textureSampler, float2(u + param0, v - param1))
        + textureMap0.Sample(textureSampler, float2(u - param0, v + param1))
        + textureMap0.Sample(textureSampler, float2(u - param0, v - param1));
        
		finalColor /= 9;
	}
    else if (mode == 10) // Combine2
    {
        float4 color0 = textureMap0.Sample(textureSampler, input.texCoord);
        float4 color1 = textureMap1.Sample(textureSampler, input.texCoord);
        finalColor = color0 + color1;
    }
    else if (mode == 11) // Night Vision
    {
        float tempGlow = input.position.y * param1 - param0 * param2;
        float glow = tempGlow - floor(tempGlow);
        
        float4 color = textureMap0.Sample(textureSampler, input.texCoord) + (glow * 0.35f * float4(0.5f, 0.5f, 0.5f, 1.0f));
        
        finalColor = float4(0.0f, color.g, 0.0f, color.a);
    }
    
    return finalColor;
}