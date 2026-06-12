#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class IntersectData
	{
	public:
		IntersectData() = default;
		IntersectData(bool doesIntersect, const Math::Vector3& direction, float penetration = 0.0f)
			: mDoesIntersect(doesIntersect), mDirection(direction), mPenetration(penetration)
		{
		}

		bool GetDoesIntersect() const { return mDoesIntersect; }
		float GetDistance() const { return Math::Magnitude(mDirection); }
		const Math::Vector3& GetDirection() const { return mDirection; }
		float GetPenetration() const { return mPenetration; }

		Math::Vector3 tempDelta = Math::Vector3::Zero;
		float tempDist = 0.0f;
		float mPenetration = 0.0f;

		bool mDoesIntersect = false;
	private:
		Math::Vector3 mDirection = Math::Vector3::Zero;
	};
}