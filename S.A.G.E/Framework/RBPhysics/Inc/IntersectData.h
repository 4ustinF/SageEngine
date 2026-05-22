#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class IntersectData
	{
	public:
		IntersectData() = default;
		IntersectData(bool doesIntersect, float distance)
			: mDoesIntersect(doesIntersect), mDistance(distance)
		{
		}

		bool GetDoesIntersect() const { return mDoesIntersect; }
		float GetDistance() const { return mDistance; }

	private:
		bool mDoesIntersect = false;
		float mDistance = 0.0f;
	};
}