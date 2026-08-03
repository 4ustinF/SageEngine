#pragma once

#include "Component.h"

namespace SAGE
{
	class RigidBodyComponent final : public Component // https://docs.unity3d.com/6000.5/Documentation/ScriptReference/Rigidbody.html
	{
	public:
		SET_TYPE_ID(ComponentId::RigidBody)
		MEMORY_POOL_DECLARE

		void DebugUI() override;
		void OnEnable() override;
		void OnDisable() override;

		// Getters
		bool IsKinematic() const;

		// Setters
		void SetMass(float mass);
		void SetDrag(float drag);
		void SetAngularDrag(float angularDrag);
		void SetUseGravity(bool useGravity);
		void SetIsKinematic(bool isKinematic);

		void SetConstraints(const SAGE::Math::Vector3Int& pos, const SAGE::Math::Vector3Int& rot);

	private:
		float mMass = 0.0f;
		float mDrag = 0.0f;
		float mAngularDrag = 0.05f;
		bool mUseGravity = true;
		bool mIsKinematic = false; // If true, the rigid body will not be affected by physics forces and will only move when explicitly moved by the user.
		// Interpolate
		// Collision Detection

		bool mFreezePositionX = false;
		bool mFreezePositionY = false;
		bool mFreezePositionZ = false;
		bool mFreezeRotationX = false;
		bool mFreezeRotationY = false;
		bool mFreezeRotationZ = false;

		// Mass
		// Drag
		// Angular Drag
		// Use Gravity
		// Is Kinematic
		// Interpolate
		// Collision Detection
		// Constraints
			// Freeze Position X
			// Freeze Position Y
			// Freeze Position Z
			// Freeze Rotation X
			// Freeze Rotation Y
			// Freeze Rotation Z

		// Info
		// Speed
		// Velocity
		// Angular Velocity

	};
}