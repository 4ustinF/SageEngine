#pragma once

namespace SAGE::Math
{
	struct Cylinder
	{
		Vector3 center = Vector3::Zero;
		int slices = 16;
		int rings = 2;
		float radius = 1.0f;
		float height = 2.0f;
		bool hasLid = true;
	};
}