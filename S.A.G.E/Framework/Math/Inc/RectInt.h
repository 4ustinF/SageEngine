#pragma once

namespace SAGE::Math
{
	struct RectInt
	{
		union
		{
			struct { int left, top; };
			SAGE::Math::Vector2Int min;
		};
		union
		{
			struct { int right, bottom; };
			SAGE::Math::Vector2Int max;
		};

		constexpr RectInt() : RectInt{ 0, 0, 1, 1 } {}
		constexpr RectInt(int left, int top, int right, int bottom) : left{ left }, top{ top }, right{ right }, bottom{ bottom } {}
	};

}