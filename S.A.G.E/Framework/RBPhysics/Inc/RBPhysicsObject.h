#pragma once

#include "Common.h"
#include "Collider.h"

namespace SAGE::RBPhysics
{
	class RBPhysicsObject
	{
	public:
		explicit RBPhysicsObject(std::unique_ptr<Collider> collider, float mass = 0) :
			mPosition(collider->GetCenter()),
			mOldPosition(collider->GetCenter()),
			mCollider(std::move(collider)),
			mMass(mass),
			mOrientation(collider->GetOrientation())
		{
			UpdateInverseMass();
		}
		RBPhysicsObject(RBPhysicsObject& other);
		RBPhysicsObject& operator=(const RBPhysicsObject& other) = default;
		virtual ~RBPhysicsObject();

		void DebugDraw(bool fillDebugShapes);

		void Integrate(float deltaTime);
		void ResolveCollision(const std::unique_ptr<RBPhysicsObject>& otherObject, const IntersectData& intersectData); // Resolving with non static objects.
		void ResolveCollision(const IntersectData& intersectData); // Resolving with static objects.

		// Getters
		float GetMass() const { return mMass; }
		float GetInverseMass() const { return mInverseMass; }
		float GetDrag() const { return mDrag; }
		float GetAngularDrag() const { return mAngularDrag; }
		bool GetUseGravity() const { return mUseGravity; }
		const Math::Vector3& GetPosition() const { return mPosition; }
		Math::Vector3 GetLocalPosition(const Math::Vector3& worldPos);
		Math::Vector3 GetWorldPosition(const Math::Vector3& localPos);
		const Math::Vector3& GetVelocity() const { return mVelocity; }
		const Math::Vector3& GetAngularVelocity() const { return mAngularVelocity; }
		const Math::Quaternion& GetOrientation() const { return mOrientation; }
		const Math::Vector3& GetAngularAcceleration() const { return mAngularAcceleration; }

		// Setters
		void SetMass(float mass) { mMass = mass; }
		void SetDrag(float drag) { mDrag = drag; }
		void SetAngularDrag(float angularDrag) { mAngularDrag = angularDrag; }
		void SetUseGravity(bool useGravity) { mUseGravity = useGravity; }
		void SetPosition(const Math::Vector3& position) { mPosition = position; }
		void SetVelocity(const Math::Vector3& velocity) { mVelocity = velocity; }
		void SetAngularVelocity(const Math::Vector3& velocity) { mAngularVelocity = velocity; }
		void SetOrientation(const Math::Quaternion& orientation) { mOrientation = orientation; }

		void ApplyForce(const Math::Vector3& force);
		void ApplyImpulse(const Math::Vector3& impulse);

		Collider* GetCollider() { return mCollider.get(); }
		const Collider* GetCollider() const { return mCollider.get(); }

		void UpdateInverseMass() { mInverseMass = mMass != 0.0f ? 1.0f / mMass : 0.0f; } // static objects often have mass 0

	private:
		void ResolveCollisionInternal(const Math::Vector3& otherVelocity, float totalInvMass, float myShare, const IntersectData& intersectData);

		float mMass = 1.0f;
		float mInverseMass = 1.0f;
		float mDrag = 0.0f;
		float mAngularDrag = 0.05f;
		bool mUseGravity = true;

		Math::Vector3 mPosition = Math::Vector3::Zero;
		Math::Vector3 mOldPosition = Math::Vector3::Zero;
		Math::Vector3 mVelocity = Math::Vector3::Zero;
		Math::Vector3 mAngularVelocity = Math::Vector3::Zero;

		Math::Quaternion mOrientation = Math::Quaternion::Identity;
		Math::Vector3 mAngularAcceleration = Math::Vector3::Zero;

		Math::Matrix3 mInertia = Math::Matrix3::Identity;
		Math::Matrix3 mInverseInertia = Math::Matrix3::Identity;

		std::unique_ptr<Collider> mCollider = nullptr; // object owns its collider's lifetime
	};
}