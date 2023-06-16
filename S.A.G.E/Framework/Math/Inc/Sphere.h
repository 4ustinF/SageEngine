#pragma once

namespace SAGE::Math
{
	struct Sphere
	{
		Vector3 center = Vector3::Zero;
		float radius = 1.0f;
		int slices = 21;
		int rings = 21;
	};
}