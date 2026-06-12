#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class IntersectData
	{
	public:
		IntersectData() = default;
		IntersectData(bool doesIntersect, const Math::Vector3& normal, Math::Vector3 contactPoint, float penetration)
			: mDoesIntersect(doesIntersect), mNormal(normal), mContactPoint(contactPoint), mPenetration(penetration)
		{
		}

		bool GetDoesIntersect() const { return mDoesIntersect; }
		//float GetDistance() const { return Math::Magnitude(mNormal); }
		const Math::Vector3& GetNormal() const { return mNormal; }
		float GetPenetration() const { return mPenetration; }

	private:
		bool mDoesIntersect = false;
		Math::Vector3 mNormal = Math::Vector3::Zero;
		Math::Vector3 mContactPoint = Math::Vector3::Zero;
		float mPenetration = 0.0f;
	};
}