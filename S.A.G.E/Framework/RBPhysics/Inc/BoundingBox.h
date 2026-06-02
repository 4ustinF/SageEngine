#pragma once

#include "Common.h"
#include "Collider.h"

namespace SAGE::RBPhysics
{
	class BoundingBox : public Collider
	{
	public:
		BoundingBox(const Math::Vector3& center, const Math::Vector3& extend)
			: Collider(Collider::TYPE_BOX), mCenter(center), mExtend(extend)
		{
		}

		void DebugDraw(SAGE::Math::Quaternion orientation, bool fillDebugShapes) override;

		//IntersectData IntersectBoundingSphere(const BoundingSphere& other) const;
		void Transform(const Math::Vector3& translation) override;

		Math::Vector3 GetCenter() const override { return mCenter; }
		Math::Vector3 GetExtend() const { return mExtend; }

	private:
		Math::Vector3 mCenter = Math::Vector3::Zero;
		Math::Vector3 mExtend = Math::Vector3::One;

	};
}