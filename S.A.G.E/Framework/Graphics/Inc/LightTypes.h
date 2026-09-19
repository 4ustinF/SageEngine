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
		float range = 10.0f;
		Math::Vector3 direction = Math::Normalize({ 0.1f, -1.0f,  0.1f }); // TODO: 
		float innerConeAngle = 0.17453292519f; // 10.0f * Constants::DegToRad;
		Math::Vector3 attenuation = Math::Vector3(1.0f, 0.045f, 0.0075f);
		float outerConeAngle = 1.2217304764f; // 70.0f * Constants::DegToRad;
		Color ambient = Color(0.05f, 0.05f, 0.05f, 1.0f);	// TODO:
		Color diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);		// TODO:
		Color specular = Color(1.0f, 1.0f, 1.0f, 1.0f);		// TODO:
	};
}