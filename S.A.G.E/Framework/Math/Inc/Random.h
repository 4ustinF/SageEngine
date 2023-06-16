#pragma once

namespace SAGE::Math::Random
{
	int UniformInt(int min = 0, int max = std::numeric_limits<int>::max());

	float UniformFloat(float min = 0.0f, float max = 1.0f);
	float NormalUniform(float mean, float max);

	Vector2 InsideUnitCircle();
	Vector2 OnUnitCircle();

	Vector3 InsideUnitSphere();
	Vector3 OnUnitSphere();
}