#pragma once

#include "Common.h"
#include "BoundingSphere.h" // TODO: Move out.

namespace SAGE::RBPhysics
{
	class RBPhysicsObject
	{
	public:
		RBPhysicsObject() = default;
		RBPhysicsObject(Math::Vector3 position, Math::Vector3 velocity, float radius) :
			mPosition(position),
			mVelocity(velocity),
			mRadius(radius),
			mBoundingSphere(position, radius)
		{
		}

		void Integrate(float deltaTime);

		const Math::Vector3& GetPosition() const { return mPosition; }
		const Math::Vector3& GetVelocity() const { return mVelocity; }

		void SetPosition(const Math::Vector3& position) { mPosition = position; }
		void SetVelocity(const Math::Vector3& velocity) { mVelocity = velocity; }
		void SetRadius(float radius) { mRadius = radius; }

		const Collider& GetCollider() // TODO: 
		{
			mBoundingSphere = BoundingSphere(mPosition, mRadius);
			return mBoundingSphere;
		}

	private:
		Math::Vector3 mPosition = Math::Vector3::Zero;
		Math::Vector3 mVelocity = Math::Vector3::Zero;

		// TODO: Remove later
		float mRadius = 0.0f;

		// TODO: This is temp.
		BoundingSphere mBoundingSphere;
	};
}