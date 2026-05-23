#pragma once

#include "Common.h"
#include "BoundingSphere.h" // TODO: Move out.

namespace SAGE::RBPhysics
{
	class RBPhysicsObject
	{
	public:
		RBPhysicsObject(Collider* collider, const Math::Vector3& velocity, const Math::Vector3& acc) :
			mPosition(collider->GetCenter()),
			mOldPosition(collider->GetCenter()),
			mVelocity(velocity),
			mCollider(collider),
			mAcceleration(acc)
		{
			mMass = 1.0f;
			mInverseMass = 1.0f / mMass;
			mInertia = Math::Matrix3::Identity;
			mInverseInertia = Math::Inverse(mInertia);
		}
		RBPhysicsObject(const RBPhysicsObject& other);
		void operator=(RBPhysicsObject other);
		virtual ~RBPhysicsObject();

		void Integrate(float deltaTime);

		const Math::Vector3& GetPosition() const { return mPosition; }
		const Math::Vector3& GetVelocity() const { return mVelocity; }
		const Math::Quaternion& GetOrientation() const { return mOrientation; }

		void SetVelocity(const Math::Vector3& velocity) { mVelocity = velocity; }

		const Collider& GetCollider() // TODO: This is temp.
		{
			Math::Vector3 translation = mPosition - mOldPosition;
			mOldPosition = mPosition;
			mCollider->Transform(translation);

			return *mCollider;
		}

	private:
		float mMass = 0.0f;
		float mInverseMass = 0.0f;

		Math::Vector3 mPosition = Math::Vector3::Zero;
		Math::Vector3 mOldPosition = Math::Vector3::Zero;
		Math::Vector3 mVelocity = Math::Vector3::Zero;
		Math::Vector3 mAngularVelocity = Math::Vector3::Zero;

		Math::Quaternion mOrientation= Math::Quaternion::Identity;
		Math::Vector3 mAcceleration = Math::Vector3::Zero;
		Math::Vector3 mAngularAcceleration = Math::Vector3::Zero;

		Math::Matrix3 mInertia = Math::Matrix3::Identity;
		Math::Matrix3 mInverseInertia = Math::Matrix3::Identity;

		Collider* mCollider;
	};
}