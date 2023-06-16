#pragma once

#include "Component.h"

namespace SAGE
{
	class ColliderComponent;

	class RigidBodyComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::RigidBody)
		MEMORY_POOL_DECLARE

		void DebugUI() override;
		void OnEnable() override;
		void OnDisable() override;

		void SetMass(float mass);
		bool IsDynamic() const;

		btRigidBody* GetRigidBody();
		void SetConstraints(const SAGE::Math::Vector3Int& pos, const SAGE::Math::Vector3Int& rot);

		void AddCollisionMask(CollisionGroups collisionGroup);
		void RemoveCollisionMask(CollisionGroups collisionGroup);
		void SetCollisionGroup(CollisionGroups collisionGroup);

	private:
		btRigidBody* mRigidBody = nullptr;
		btGeneric6DofSpringConstraint* mConstraint = nullptr;
		float mMass = 0.0f;
		bool mIsdynamic = false;
		bool mIsInitialized = false;

		void UpdateConstraint();
		bool mFreezePositionX = false;
		bool mFreezePositionY = false;
		bool mFreezePositionZ = false;
		bool mFreezeRotationX = false;
		bool mFreezeRotationY = false;
		bool mFreezeRotationZ = false;
	};
}