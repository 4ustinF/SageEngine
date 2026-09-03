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

	inline constexpr size_t MaxSpotLights = 4;
	struct SpotLight
	{
		Math::Vector3 position;
		float range;
		Math::Vector3 direction;
		float innerConeAngle;
		Math::Vector3 attenuation;
		float outerConeAngle;
		Color ambient;
		Color diffuse;
		Color specular;
	};
}