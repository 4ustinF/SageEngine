#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class IntersectData;

	class BoundingSphere
	{
	public:
		BoundingSphere() = default;
		BoundingSphere(Math::Vector3 center, float radius)
			: mCenter(center), mRadius(radius)
		{
		}

		IntersectData IntersectBoundingSphere(const BoundingSphere& other);

		const Math::Vector3& GetCenter() const { return mCenter; }
		float GetRadius() const { return mRadius; }

	private:
		Math::Vector3 mCenter = Math::Vector3::Zero;
		float mRadius = 0.0f;
	};
}