#pragma once

namespace SAGE::Math
{
	struct Quaternion
	{
		float w, x, y, z;

		constexpr Quaternion() noexcept : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
		constexpr Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

		const static Quaternion Zero;
		const static Quaternion Identity;

		static Quaternion RotationAxis(const Vector3& axis, float rad); // Use Slides
		static Quaternion RotationEuler(const Vector3& eulerAngles); // Use the WWW
		static Quaternion RotationMatrix(const Matrix4& m); // Use the WWW
		static Quaternion RotationLook(const Vector3& direction, const Vector3& up = Vector3::YAxis); // Use RotationMatrix
		static Quaternion RotationFromTo(const Vector3& from, const Vector3& to); // Use RotationAxis

		constexpr Quaternion operator-() const
		{
			return { -w, -x, -y, -z };
		}

		constexpr Quaternion operator+(const Quaternion& rhs) const
		{
			return { w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z };
		}

		constexpr Quaternion operator-(const Quaternion& rhs) const
		{
			return { w - rhs.w, x - rhs.x, y - rhs.y, z - rhs.z };
		}

		constexpr Quaternion operator*(const Quaternion& rhs) const
		{
			Quaternion result
			{
				(rhs.w * w) - (rhs.x * x) - (rhs.y * y) - (rhs.z * z),
				(rhs.w * x) + (rhs.x * w) - (rhs.y * z) + (rhs.z * y),
				(rhs.w * y) + (rhs.x * z) + (rhs.y * w) - (rhs.z * x),
				(rhs.w * z) - (rhs.x * y) + (rhs.y * x) + (rhs.z * w)
			};
			return result;
		}

		constexpr Quaternion operator*(float s) const
		{
			return { w * s, x * s, y * s, z * s };
		}

		constexpr Quaternion operator/(float s) const
		{
			return { w / s, x / s, y / x, z / s };
		}
	};
}