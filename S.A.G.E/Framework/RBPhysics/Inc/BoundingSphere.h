#pragma once

#include "Common.h"
#include "Collider.h"


namespace SAGE::RBPhysics
{
	class BoundingBox;

	class BoundingSphere : public Collider
	{
	public:
		BoundingSphere(const Math::Vector3& center, float radius)
			: Collider(Collider::TYPE_SPHERE), mRadius(radius)
		{
			mCenter = center;
		}

		void DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes) override;

		void Transform(const Math::Vector3& translation) override;
		float GetRadius() const { return mRadius; }
		float GetRadiusSquared() const { return mRadius * mRadius; }
		Math::Vector3 GetClosestPoint(const Collider& otherCollider) const;

		IntersectData IntersectBoundingSphere(const BoundingSphere& other) const;
		IntersectData IntersectBoundingSphere(const BoundingBox& other);// const;

	private:
		float mRadius = 0.0f;

		// Debug Draw
		int mDebugDivisions = 16;
	};
}