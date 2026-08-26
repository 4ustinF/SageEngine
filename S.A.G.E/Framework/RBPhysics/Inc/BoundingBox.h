#pragma once

#include "Common.h"
#include "Collider.h"

namespace SAGE::RBPhysics
{
	class BoundingBox : public Collider
	{
	public:
		BoundingBox(const Math::Vector3& center, const Math::Vector3& extend, const Math::Quaternion& orientation)
			: Collider(Collider::TYPE_BOX), mExtend(extend)
		{
			mCenter = center;
			mOrientation = orientation;
			mIsAxisAligned = mOrientation == Math::Quaternion::Identity; // TODO: Other orientations as well.
		}

		void DebugDraw(bool fillDebugShapes) override;

		//IntersectData IntersectBoundingSphere(const BoundingSphere& other) const;
		void Transform(const Math::Vector3& translation) override;

		const Math::Vector3& GetExtend() const { return mExtend; }
		Math::Vector3 GetExtend() { return mExtend; }
		Math::Vector3 GetMinExtend() const { return mCenter - mExtend; }
		Math::Vector3 GetMaxExtend() const { return mCenter + mExtend; }
		bool IsAxisAligned() const { return mIsAxisAligned; }

		void SetOrientation(const SAGE::Math::Quaternion& orientation) override;
		void SetExtend(const Math::Vector3& extend) { mExtend = extend; }

	private:
		Math::Vector3 mExtend = Math::Vector3::One;
		bool mIsAxisAligned = false;
	};
}