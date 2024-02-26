#pragma once

namespace SAGE::Math
{
	struct Vector2
	{
		union
		{
			struct { float x, y; };
			std::array<float, 2> v; //Same as float v[2], but much better 
		};

		//Constructors
		constexpr Vector2() noexcept : Vector2(0.0f) {}
		explicit constexpr Vector2(float f) noexcept : Vector2(f, f) {}
		constexpr Vector2(float x, float y) noexcept : x(x), y(y) { }

		//Commonly used vectors
		const static Vector2 Zero;
		const static Vector2 One;
		const static Vector2 XAxis;
		const static Vector2 YAxis;

		constexpr Vector2 operator-() const { return { -x, -y }; }
		constexpr Vector2 operator+(const Vector2& v) const { return { x + v.x, y + v.y }; }
		constexpr Vector2 operator+(const float f) const { return { x + f, y + f }; }
		constexpr Vector2 operator-(const Vector2& v) const { return { x - v.x, y - v.y }; }
		constexpr Vector2 operator*(const float& f) const { return { x * f, y * f }; }
		constexpr Vector2 operator/(const float& f) const { return { x / f, y / f }; }

		Vector2& operator+=(const Vector2& v) { x += v.x, y += v.y; return *this; }
		Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
		bool operator==(const Vector2& v) const { return x == v.x && y == v.y; }
		Vector2& operator*=(float f) { x *= f; y *= f; return *this; }
		Vector2& operator/=(float f) { x /= f; y /= f; return *this; }
	};

}

