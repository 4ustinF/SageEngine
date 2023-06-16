#pragma once

namespace SAGE::Math
{
	struct AABB
	{
		AABB() = default;
		AABB(const Vector3& center, const Vector3& extend)
			: center(center), extend(extend)
		{}

		Vector3 center = Vector3::Zero;
		Vector3 extend = Vector3::One;

		constexpr Vector3 Min() const { return center - extend; }
		constexpr Vector3 Max() const { return center + extend; }
	};
}