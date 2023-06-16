#pragma once

namespace SAGE::Math
{
	struct Vector3Int
	{
		union
		{
			struct { int x, y, z; };
			std::array<int, 3> v; //Same as int v[3], but much better 
		};

		//Constructors
		constexpr Vector3Int() noexcept : Vector3Int(0) {}
		explicit constexpr Vector3Int(int i) noexcept : Vector3Int(i, i, i) {}
		constexpr Vector3Int(int x, int y, int z) noexcept : x(x), y(y), z(z) { }

		//Commonly used vectors
		const static Vector3Int Zero;
		const static Vector3Int One;
		const static Vector3Int XAxis;
		const static Vector3Int YAxis;
		const static Vector3Int ZAxis;

		constexpr Vector3Int operator-() const { return { -x, -y, -z }; }
		constexpr Vector3Int operator+(const Vector3Int& v) const { return { x + v.x, y + v.y, z + v.z }; }
		constexpr Vector3Int operator-(const Vector3Int& v) const { return { x - v.x, y - v.y, z - v.z }; }
		constexpr Vector3Int operator*(const int& i) const { return { x * i, y * i, z * i }; }
		constexpr Vector3Int operator/(const int& i) const { return { x / i, y / i, z / i }; }

		Vector3Int& operator+=(const Vector3Int& v) { x += v.x, y += v.y, z += v.z; return *this; }
		Vector3Int& operator-=(const Vector3Int& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		bool operator==(const Vector3Int& v) { return x == v.x && y == v.y && z == v.z; }
		bool operator!=(const Vector3Int& v) { return x != v.x || y != v.y || z != v.z; }
		Vector3Int& operator*=(int i) { x *= i; y *= i; z *= i; return *this; }
		Vector3Int& operator/=(int i) { x /= i; y /= i; z /= i; return *this; }
	};

}

