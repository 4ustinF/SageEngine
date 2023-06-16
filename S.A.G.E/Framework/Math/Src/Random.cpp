#include "Precompiled.h"
#include "SAGEmath.h"

using namespace SAGE::Math;

namespace
{
	std::random_device myRandomDevice{};
	std::mt19937 myRandomEngine{ myRandomDevice() };
}

int Random::UniformInt(int min, int max)
{
	return std::uniform_int_distribution<>{min, max}(myRandomEngine);
}

float Random::UniformFloat(float min, float max)
{
	return std::uniform_real_distribution<float>{min, max}(myRandomEngine);
}

float Random::NormalUniform(float mean, float max)
{
	return std::normal_distribution<float>{mean, max}(myRandomEngine);
}

Vector2 Random::InsideUnitCircle()
{
	return {
			UniformFloat(-1.0f, 1.0f) + 0.001f, // Ensure non-zero vector
			UniformFloat(-1.0f, 1.0f)
		};
}

Vector2 Random::OnUnitCircle()
{
	return Normalize(
		{
			UniformFloat(-1.0f, 1.0f) + 0.001f, // Ensure non-zero vector
			UniformFloat(-1.0f, 1.0f)
		});
}

Vector3 Random::InsideUnitSphere()
{
	return {
		UniformFloat(-1.0f, 1.0f),
		UniformFloat(-1.0f, 1.0f),
		UniformFloat(-1.0f, 1.0f)
	};
}

Vector3 Random::OnUnitSphere()
{
	return Normalize(
		{
			UniformFloat(-1.0f, 1.0f) + 0.001f, // Ensure non-zero vector
			UniformFloat(-1.0f, 1),
			UniformFloat(-1.0f, 1)
		});
}