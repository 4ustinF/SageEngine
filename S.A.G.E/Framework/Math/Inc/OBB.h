#pragma once

namespace SAGE::Math
{
	struct OBB
	{
		OBB() = default;
		OBB(const Vector3& center, const Vector3& extend, const Quaternion& rotation)
			: center(center), extend(extend), rotation(rotation) {
		}

		Vector3 center = Vector3::Zero;
		Vector3 extend = Vector3::One;
		Quaternion rotation = Quaternion::Identity;
	};
}