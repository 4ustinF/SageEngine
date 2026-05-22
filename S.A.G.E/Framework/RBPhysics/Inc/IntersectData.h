#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class IntersectData
	{
	public:
		IntersectData() = default;
		IntersectData(bool doesIntersect, const Math::Vector3& direction)
			: mDoesIntersect(doesIntersect), mDirection(direction)
		{
		}

		bool GetDoesIntersect() const { return mDoesIntersect; }
		float GetDistance() const { return Math::Magnitude(mDirection); }
		const Math::Vector3& GetDirection() const { return mDirection; }

	private:
		bool mDoesIntersect = false;
		Math::Vector3 mDirection = Math::Vector3::Zero;
	};
}