#pragma once

namespace SAGE::Math
{
	struct Ray
	{
		Vector3 origin = Vector3::Zero;
		Vector3 direction = Vector3::ZAxis;

		constexpr Vector3 GetPoint(float distance) const 
		{
			return origin + (direction * distance);
		}
	};
}