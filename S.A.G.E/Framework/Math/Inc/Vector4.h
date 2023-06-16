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

		constexpr Vector4 operator-() const { return { -x, -y, -z, -w }; }
		constexpr Vector4 operator+(const Vector4& v) const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
		constexpr Vector4 operator-(const Vector4& v) const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
		constexpr Vector4 operator*(const float& f) const { return { x * f, y * f, z * f, w * f }; }
		constexpr Vector4 operator/(const float& f) const { return { x / f, y / f, z / f, w / f }; }

		Vector4& operator+=(const Vector4& v) { x += v.x, y += v.y, z += v.z, w += v.w; return *this; }
		Vector4& operator-=(const Vector4& v) { x -= v.x; y -= v.y; z -= v.z, w -= v.w; return *this; }
		bool operator==(const Vector4& v) { return x == v.x && y == v.y && z == v.z && w == v.w; }
		Vector4& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
		Vector4& operator/=(float f) { x /= f; y /= f; z /= f; w /= f; return *this; }
	};
}