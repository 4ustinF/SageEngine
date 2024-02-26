#pragma once

namespace SAGE::Math
{
	struct Vector3
	{
		union
		{
			struct { float x, y, z; };
			std::array<float, 3> v; //Same as float v[3], but much better 
		};

		//Constructors
		constexpr Vector3() noexcept : Vector3(0.0f) {}
		explicit constexpr Vector3(float f) noexcept : Vector3(f, f, f) {}
		constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) { }

		//Commonly used vectors
		const static Vector3 Zero;
		const static Vector3 One;
		const static Vector3 XAxis;
		const static Vector3 YAxis;
		const static Vector3 ZAxis;

		constexpr Vector3 operator-() const { return { -x, -y, -z }; }
		constexpr Vector3 operator+(const Vector3& v) const { return { x + v.x, y + v.y, z + v.z }; }
		constexpr Vector3 operator-(const Vector3& v) const { return { x - v.x, y - v.y, z - v.z }; }
		constexpr Vector3 operator*(const float& f) const { return { x * f, y * f, z * f }; }
		constexpr Vector3 operator/(const float& f) const { return { x / f, y / f, z / f }; }

		Vector3& operator+=(const Vector3& v) { x += v.x, y += v.y, z += v.z; return *this; }
		Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		bool operator==(const Vector3& v) const { return x == v.x && y == v.y && z == v.z;}
		Vector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
		Vector3& operator/=(float f) { x /= f; y /= f; z /= f; return *this; }
	};

}

