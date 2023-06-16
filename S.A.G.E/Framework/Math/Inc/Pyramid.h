#pragma once

namespace SAGE::Math
{
	struct Pyramid
	{
		Pyramid() = default;
		Pyramid(const Vector3& center, const float length, const float width, const float height, const Quaternion& rotation = Quaternion::Identity)
			: center(center), length(length), width(width), height(height), rotation(rotation)
		{}

		Vector3 center = Vector3::Zero;
		Quaternion rotation = Quaternion::Identity;
		float length = 1.0f;
		float width = 1.0f;
		float height = 1.0f;
	};
}