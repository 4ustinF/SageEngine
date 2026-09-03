#pragma once

#include "Colors.h"

namespace SAGE::Graphics
{
	struct DirectionalLight
	{
		Math::Vector3 direction;
		float padding = 0.0f;
		Color ambient;
		Color diffuse;
		Color specular;
	};

	// Attenuation:
	// how much a light's intensity falls off with distance
		// the further a surface is from the light, the dimmer it should look. 
		// Without it, a point/spot light would illuminate everything at full brightness no matter how far away,
		// which looks wrong and flat.

	inline constexpr size_t MaxSpotLights = 4;
	struct SpotLight
	{
		Math::Vector3 position;
		float range;
		Math::Vector3 direction;
		float innerConeAngle;
		Math::Vector3 attenuation; // {1.0f, 0.045f, 0.0075f}
		float outerConeAngle;
		Color ambient;
		Color diffuse;
		Color specular;
	};
}