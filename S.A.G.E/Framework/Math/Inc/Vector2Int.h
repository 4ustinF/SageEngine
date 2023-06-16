#pragma once

namespace SAGE::Math
{
	struct Vector2Int
	{
		union
		{
			struct { int x, y; };
			std::array<int, 2> v; //Same as int v[2], but much better 
		};

		//Constructors
		constexpr Vector2Int() noexcept : Vector2Int(0) {}
		explicit constexpr Vector2Int(int i) noexcept : Vector2Int(i, i) {}
		constexpr Vector2Int(int x, int y) noexcept : x(x), y(y) { }

		//Commonly used vectors
		const static Vector2Int Zero;
		const static Vector2Int One;
		const static Vector2Int XAxis;
		const static Vector2Int YAxis;

		constexpr Vector2Int operator-() const { return { -x, -y }; }
		constexpr Vector2Int operator+(const Vector2Int& v) const { return { x + v.x, y + v.y }; }
		constexpr Vector2Int operator-(const Vector2Int& v) const { return { x - v.x, y - v.y }; }
		constexpr Vector2Int operator-(const int& i) const { return { x - i, y - i }; }
		constexpr Vector2Int operator+(const int& i) const { return { x + i, y + i }; }
		constexpr Vector2Int operator*(const int& i) const { return { x * i, y * i }; }
		constexpr Vector2Int operator/(const int& i) const { return { x / i, y / i }; }

		Vector2Int& operator+=(const Vector2Int& v) { x += v.x, y += v.y; return *this; }
		Vector2Int& operator-=(const Vector2Int& v) { x -= v.x; y -= v.y; return *this; }
		bool operator==(const Vector2Int& v) { return x == v.x && y == v.y; }
		bool operator!=(const Vector2Int& v) { return x != v.x || y != v.y; }
		Vector2Int& operator*=(int i) { x *= i; y *= i; return *this; }
		Vector2Int& operator/=(int i) { x /= i; y /= i; return *this; }
	};

}

