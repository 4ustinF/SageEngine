#include "Precompiled.h"
#include "BPhysicsComponent.h"
#include "BPhysicsService.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "GameWorld.h"
#include "ColliderComponent.h"

#include "ModelComponent.h"

using namespace SAGE;

MEMORY_POOL_DEFINE(BPhysicsComponent, 100);

void BPhysicsComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	mRigidBodyComponent = GetOwner().GetComponent<RigidBodyComponent>();
	mColliderComponent = GetOwner().GetComponent<ColliderComponent>();
	mModelComponent = GetOwner().GetComponent<ModelComponent>();

	auto physicsService = GetOwner().GetWorld().GetService<BPhysicsService>();
	physicsService->Register(this);
}

void BPhysicsComponent::Terminate()
{
	auto physicsService = GetOwner().GetWorld().GetService<BPhysicsService>();
	physicsService->Unregister(this);

	mTransformComponent = nullptr;
	mRigidBodyComponent = nullptr;
	mColliderComponent = nullptr;
	mModelComponent = nullptr;
}

void BPhysicsComponent::UpdatePhysics(float deltaTime)
{
	if (!GetOwner().IsActive())
	{
		return;
	}

	if (mRigidBodyComponent != nullptr)
	{
		auto& rigidBodyTransform = mRigidBodyComponent->GetRigidBody()->getWorldTransform();
		ConvertToTransform(rigidBodyTransform, *mTransformComponent, mColliderComponent->GetCenter());

		if (mModelComponent)
		{
			const auto angles = mModelComponent->GetRotation();
			const auto rotation = Math::Quaternion::RotationEuler(angles);
			mTransformComponent->rotation = mTransformComponent->rotation * rotation;
		}
	}
}

void BPhysicsComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("BPhysics Component##BPhysicsComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
	}
	//const auto& rigidBodyTransform = mRigidBodyComponent->GetRigidBody()->getWorldTransform();
	//const auto& aabb = mColliderComponent->GetWorldAABB();
	//const auto& r = rigidBodyTransform.getRotation();
	//const Math::Quaternion rotation = Math::Quaternion(r.x(), r.y(), r.z(), r.w());
	//Graphics::SimpleDraw::AddOBB(aabb.center, aabb.extend, rotation, Graphics::Colors::Green);
}
