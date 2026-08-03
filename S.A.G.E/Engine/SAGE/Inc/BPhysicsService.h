#pragma once

#include "Service.h"

namespace SAGE
{
	class BPhysicsComponent;
	class TransformComponent;
	class ColliderComponent;
	class RigidBodyComponentOld;
	class PhysicsDebugDraw;

	class BPhysicsService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::BPhysics)

		void Initialize() override;
		void Terminate() override;
		void Update(float deltaTime) override;
		void DebugUI() override;

		btDiscreteDynamicsWorld* GetDynamicsWorld() { return mDynamicWorld; }

		void SetGravity(float gravity);
		void SetMaxSubSteps(int steps) { mMaxSubSteps = steps; }

		friend class BPhysicsComponent;
		void AddRigidBody(RigidBodyComponentOld* rigidBodyComponent);
		void RemoveRigidBody(RigidBodyComponentOld* rigidBodyComponent);

	private:
		friend class BPhysicsComponent;
		void Register(BPhysicsComponent* physicsComponent);
		void Unregister(BPhysicsComponent* physicsComponent);

		struct Entry
		{
			ColliderComponent* colliderComponent = nullptr;
			RigidBodyComponentOld* rigidBodyComponent = nullptr;
			BPhysicsComponent* bPhysicsComponent = nullptr;
			TransformComponent* transformComponent = nullptr;
		};

		using PhysicsEntries = std::vector<Entry>;
		PhysicsEntries mPhysicsEntries;

		btBroadphaseInterface* mInterface = nullptr;
		btCollisionDispatcher* mDispatcher = nullptr;
		btDefaultCollisionConfiguration* mCollisionConfiguration = nullptr;
		btDiscreteDynamicsWorld* mDynamicWorld = nullptr;
		btSequentialImpulseConstraintSolver* mSolver = nullptr;
		PhysicsDebugDraw* mPhysicsDebugDraw = nullptr;

		bool mRenderDebugUI = false;
		float mGravity = -9.81f;
		int mMaxSubSteps = 1; // 10
	};
}