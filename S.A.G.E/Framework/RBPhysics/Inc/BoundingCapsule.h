#pragma once

#include "Common.h"
#include "Collider.h"

namespace SAGE::RBPhysics
{
	class BoundingCapsule : public Collider
	{
	public:
		BoundingCapsule(const Math::Vector3& center, float radius, float height)
			: Collider(Collider::TYPE_CAPSULE), mRadius(radius), mHeight(height)
		{
			mCenter = center;
		}

		void DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes) override;

		void Transform(const Math::Vector3& translation) override;
		float GetRadius() const { return mRadius; }
		float GetRadiusSquared() const { return mRadius * mRadius; }
		Math::Vector3 GetClosestPoint(const Collider& otherCollider) const;

	private:
		float mRadius = 0.5f;
		float mHeight = 2.0f;

		// Debug Draw
		int mDebugDivisions = 16;
	};
}