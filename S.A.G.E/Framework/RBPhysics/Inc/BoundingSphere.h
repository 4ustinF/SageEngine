#pragma once

#include "Common.h"
#include "Collider.h"

namespace SAGE::RBPhysics
{
	class BoundingSphere : public Collider
	{
	public:
		BoundingSphere(const Math::Vector3& center, float radius)
			: Collider(Collider::TYPE_SPHERE), mCenter(center), mRadius(radius)
		{
		}

		IntersectData IntersectBoundingSphere(const BoundingSphere& other) const;
		void Transform(const Math::Vector3& translation) override;
		Math::Vector3 GetCenter() const override { return mCenter; }

		float GetRadius() const { return mRadius; }

	private:
		Math::Vector3 mCenter = Math::Vector3::Zero;
		float mRadius = 0.0f;
	};
}