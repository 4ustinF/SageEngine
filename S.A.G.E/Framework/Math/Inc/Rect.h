#pragma once

namespace SAGE::Math
{
	struct Rect
	{
		union
		{
			struct { float left, top; };
			SAGE::Math::Vector2 min;
		};
		union
		{
			struct { float right, bottom; };
			SAGE::Math::Vector2 max;
		};

		constexpr Rect() : Rect{ 0.0f, 0.0f, 1.0f, 1.0f } {}
		constexpr Rect(float left, float top, float right, float bottom) : left{ left }, top{ top }, right{ right }, bottom{ bottom } {}
		constexpr Rect(float x, float y, float offset) : left{ x - offset }, top{ y - offset }, right{ x + offset }, bottom{ y + offset } {}
		constexpr Rect(const Vector2& pos, float offset) : left{ pos.x - offset }, top{ pos.y - offset }, right{ pos.x + offset }, bottom{ pos.y + offset } {}
		constexpr Rect(const Vector2& pos, float xOffset, float yOffset) : left{ pos.x - xOffset }, top{ pos.y - yOffset }, right{ pos.x + xOffset }, bottom{ pos.y + yOffset } {}
	};

}