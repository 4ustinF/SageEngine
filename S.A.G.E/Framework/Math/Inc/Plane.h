#pragma once

namespace SAGE::Math
{
	struct Plane
	{
		Plane() = default;
		Plane(Vector3 normal, float distance)
			: normal(normal), distance(distance)
		{
		}

		Vector3 normal = Vector3::YAxis;
		float distance = 0.0f;
	};
}