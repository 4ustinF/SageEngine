// Description: Shader for voxel like cloud system

cbuffer ConstantBuffer : register(b0)
{
    matrix wvp;
}

cbuffer SettingBuffer : register(b1)
{
	bool useFast;
}

cbuffer SkyBuffer : register(b2)
{
    float2 translation;
    float scale;
    float brightness;
};

Texture2D diffuseMap : register(t0);
Texture2D noiseMap : register(t1);

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
	if (useFast)
	{
		input.texCoord = float2(input.texCoord.x + translation.x, input.texCoord.y + translation.y);
		float4 finalColor = diffuseMap.Sample(textureSampler, input.texCoord);
		if (finalColor.a > 0.01f)
		{
			finalColor.a = brightness;
		}
    
		return finalColor;
	}
	else
	{
		// Translate the texture coordinate sampling location by the translation value.
        input.texCoord += translation;

		// Sample the texture value from the perturb texture using the translated texture coordinates.
		float4 perturbValue = noiseMap.Sample(textureSampler, input.texCoord);

		// Multiply the perturb value by the perturb scale.
		perturbValue *= scale;

		// Add the texture coordinates as well as the translation value to get the perturbed texture coordinate sampling location.
		perturbValue.rg += input.texCoord.xy + translation.x;

		// Now sample the color from the cloud texture using the perturbed sampling coordinates.
		float4 cloudColor = diffuseMap.Sample(textureSampler, perturbValue.rg);
		
		// Reduce the color cloud by the brightness value.
		cloudColor *= brightness;
		
		return cloudColor;
	}
}