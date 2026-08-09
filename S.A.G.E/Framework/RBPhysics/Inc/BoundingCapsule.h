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

	private:
		float mRadius = 0.5f;
		float mHeight = 2.0f;

		// Debug Draw
		int mDebugDivisions = 16;
	};
}