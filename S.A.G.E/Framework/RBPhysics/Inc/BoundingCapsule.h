#pragma once

#include "Common.h"
#include "Collider.h"

namespace SAGE::RBPhysics
{
	class BoundingBox;

	class BoundingCapsule : public Collider
	{
	public:
		BoundingCapsule(const Math::Vector3& center, float radius, float height)
			: Collider(Collider::TYPE_CAPSULE), mRadius(radius), mHeight(height)
		{
			mCenter = center;
		}

		void DebugDraw(Math::Quaternion orientation, bool fillDebugShapes) override;

		void Transform(const Math::Vector3& translation) override;
		float GetRadius() const { return mRadius; }
		float GetRadiusSquared() const { return mRadius * mRadius; }

		Math::Vector3 GetTopCenter() const { return mCenter + Math::Vector3(0.0f, mHeight * 0.5f, 0.0f); }
		Math::Vector3 GetBottomCenter() const { return mCenter - Math::Vector3(0.0f, mHeight * 0.5f, 0.0f); }
		Math::Vector3 GetInnerTopCenter() const { return mCenter + Math::Vector3(0.0f, mHeight * 0.5f - mRadius, 0.0f); }
		Math::Vector3 GetInnerBottomCenter() const { return mCenter - Math::Vector3(0.0f, mHeight * 0.5f - mRadius, 0.0f); }

		IntersectData IntersectBoundingCapsule(const BoundingBox& other); // TODO: Remove
		IntersectData IntersectBoundingCapsule(const BoundingCapsule& other); // TODO: Remove
		Math::Vector3 ClosestPointOnSegment(const Math::Vector3& a, const Math::Vector3& b, const Math::Vector3& point);
		Math::Vector3 ClosestPointSegmentToBox(const Math::Vector3& segA, const Math::Vector3& segB, const Math::Vector3& min, const Math::Vector3& max);

		// Computes closest points between segment (p1,q1) and (p2,q2)
		// Returns closest point on each segment via out params, and the squared distance
		float ClosestPointSegmentSegment(const Math::Vector3& p1, const Math::Vector3& q1, const Math::Vector3& p2, const Math::Vector3& q2, Math::Vector3& c1, Math::Vector3& c2);

	private:
		float mRadius = 0.5f;
		float mHeight = 2.0f;

		// Debug Draw
		int mDebugDivisions = 16;
	};
}