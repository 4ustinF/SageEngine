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

	struct SpotLight
	{
		Math::Vector3 position;
		float range;
		Math::Vector3 direction;
		float innerConeAngle;

		Color ambient;
		Color diffuse;
		Color specular;

		Math::Vector3 attenuation;
		float outerConeAngle;
	};
}