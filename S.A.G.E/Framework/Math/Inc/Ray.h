#pragma once

namespace SAGE::Math
{
	struct Ray
	{
		Ray() = default;
		Ray(const Vector3& origin, const Vector3& direction)
			: origin(origin), direction(direction) {}

		Vector3 origin = Vector3::Zero;
		Vector3 direction = Vector3::ZAxis;

		constexpr Vector3 GetPoint(float distance) const 
		{
			return origin + (direction * distance);
		}
	};

	struct RayHit
	{
		bool hit = false;
		float distance = 0.0f;
		Vector3 position;
		Vector3 normal;
	};
}