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
	};

}