#pragma once

#include "Component.h"

namespace SAGE
{
	class TransformComponent;
	class RigidBodyComponent;
	class ColliderComponent;
	class ModelComponent;

	class BPhysicsComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::BPhysics)
		MEMORY_POOL_DECLARE

		void Initialize() override;
		void Terminate() override;
		void UpdatePhysics(float deltaTime);
		void DebugUI() override;

	private:
		TransformComponent* mTransformComponent = nullptr;
		RigidBodyComponent* mRigidBodyComponent = nullptr;
		ColliderComponent* mColliderComponent = nullptr;
		ModelComponent* mModelComponent = nullptr;
	};
}