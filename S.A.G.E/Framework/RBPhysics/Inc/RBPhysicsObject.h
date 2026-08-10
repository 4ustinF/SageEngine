#pragma once

#include "Common.h"
#include "BoundingSphere.h" // TODO: Move out.

namespace SAGE::RBPhysics
{
	class RBPhysicsObject
	{
	public:
		RBPhysicsObject(Collider* collider, float mass = 0) :
			mPosition(collider->GetCenter()),
			mOldPosition(collider->GetCenter()),
			mCollider(collider),
			mMass(mass),
			mOrientation(collider->GetOrientation())
		{
			mInverseMass = 1.0f / mMass;

			//// This is for sphere body 
			//float I = 2.0f / 5.0f * mMass * mMass * 1.0f; // Radius
			//mInertia = Math::Matrix3(
			//	I, 0.0f, 0.0f,
			//	0.0f, I, 0.0f,
			//	0.0f, 0.0f, I);

			//mInverseInertia = Math::Inverse(mInertia);
		}
		RBPhysicsObject(const RBPhysicsObject& other);
		void operator=(RBPhysicsObject other);
		virtual ~RBPhysicsObject();

		void DebugDraw(bool fillDebugShapes);

		void Integrate(float deltaTime);
		void ResolveCollision(const RBPhysicsObject& otherObject, const IntersectData& intersectData); // Resolving with non static objects.
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
		float GetNormalStiffness() const { return mNormalStiffness; }
		float GetNormalDampening() const { return mNormalDampening; }
		float GetTangentialStiffness() const { return mTangentialStiffness; }
		float GetTangentialDampening() const { return mTangentialDampening; }

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
		void ApplyForceAtPoint(const Math::Vector3& force, const Math::Vector3& localPoint);
		void ApplyTorque(const Math::Vector3& torque);
		void ApplyDrag(const Math::Vector3& velocity, const Math::Vector3& dragForce);

		const Collider& GetCollider() // TODO: This is temp.
		{
			Math::Vector3 translation = mPosition - mOldPosition;
			mOldPosition = mPosition;
			mCollider->Transform(translation);

			return *mCollider;
		}

	private:
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

		float mNormalStiffness = 0.8f;
		float mNormalDampening = 0.85f;
		float mTangentialStiffness = 0.8f;
		float mTangentialDampening = 0.85f;

		Collider* mCollider;
		Math::Vector3 QuatMulVec3(const Math::Vector3& vec, const Math::Matrix3& m); // Remove;
	};
}