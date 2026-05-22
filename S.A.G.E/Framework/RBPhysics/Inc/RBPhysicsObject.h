#pragma once

#include "Common.h"
#include "BoundingSphere.h" // TODO: Move out.

namespace SAGE::RBPhysics
{
	class RBPhysicsObject
	{
	public:
		RBPhysicsObject(Collider* collider, const Math::Vector3& velocity) :
			mPosition(collider->GetCenter()),
			mOldPosition(collider->GetCenter()),
			mVelocity(velocity),
			mCollider(collider)
		{
		}
		RBPhysicsObject(const RBPhysicsObject& other);
		void operator=(RBPhysicsObject other);
		virtual ~RBPhysicsObject();

		void Integrate(float deltaTime);

		const Math::Vector3& GetPosition() const { return mPosition; }
		const Math::Vector3& GetVelocity() const { return mVelocity; }

		void SetVelocity(const Math::Vector3& velocity) { mVelocity = velocity; }

		const Collider& GetCollider() // TODO: This is temp.
		{
			Math::Vector3 translation = mPosition - mOldPosition;
			mOldPosition = mPosition;
			mCollider->Transform(translation);

			return *mCollider;
		}

	private:
		Math::Vector3 mPosition = Math::Vector3::Zero;
		Math::Vector3 mOldPosition = Math::Vector3::Zero;
		Math::Vector3 mVelocity = Math::Vector3::Zero;

		Collider* mCollider;

	};
}