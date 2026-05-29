#pragma once

namespace SAGE::Math
{
	struct Vector4
	{
		union { float x; float r; };
		union { float y; float g; };
		union { float z; float b; };
		union { float w; float a; };

		//Constructors
		constexpr Vector4() noexcept : Vector4(0.0f) {}
		explicit constexpr Vector4(float f) noexcept : Vector4(f, f, f, f) {}
		constexpr Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) { }

		constexpr Vector4 operator-() const noexcept { return { -x, -y, -z, -w }; }
		constexpr Vector4 operator+(const Vector4& v) const noexcept { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
		constexpr Vector4 operator-(const Vector4& v) const noexcept { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
		constexpr Vector4 operator*(float f) const noexcept { return { x * f, y * f, z * f, w * f }; }
		constexpr Vector4 operator/(float f) const noexcept { return { x / f, y / f, z / f, w / f }; }

		Vector4& operator+=(const Vector4& v) noexcept { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		Vector4& operator-=(const Vector4& v) noexcept { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		bool operator==(const Vector4& v) const noexcept { return x == v.x && y == v.y && z == v.z && w == v.w; }
		Vector4& operator*=(float f) noexcept { x *= f; y *= f; z *= f; w *= f; return *this; }
		Vector4& operator/=(float f) noexcept { x /= f; y /= f; z /= f; w /= f; return *this; }
	};

	// Symmetric scalar ops
	constexpr inline Vector4 operator*(float f, const Vector4& v) noexcept { return v * f; }

}