#include "Precompiled.h"
#include "CapsuleColliderComponent.h"

#include "CameraService.h"
#include "GameWorld.h"

#include "GameObject.h"
#include "RBPhysicsService.h"
#include "TransformComponent.h"

#include "RigidBodyComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(CapsuleColliderComponent, 200);

void CapsuleColliderComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	BaseColliderComponent::LoadComponentFromTemplate(value);

	if (value.HasMember("Radius"))
	{
		SetRadius(value["Radius"].GetFloat());
	}

	if (value.HasMember("Height"))
	{
		SetHeight(value["Height"].GetFloat());
	}
}

void CapsuleColliderComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	BaseColliderComponent::SaveComponentToTemplate(compObj, allocator);
	SaveNumberToTemplate(compObj, allocator, "Radius", mRadius, 0.5f);
	SaveNumberToTemplate(compObj, allocator, "Height", mHeight, 2.0f);
}

void CapsuleColliderComponent ::Initialize()
{
	BaseColliderComponent::Initialize();
}

void CapsuleColliderComponent ::Terminate()
{
	BaseColliderComponent::Terminate();
}

void CapsuleColliderComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Capsule Collider Component##CapsuleColliderComponent ", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Checkbox("Debug Fill##BoxColliderComponent", &mDebugFill);
		ImGui::Checkbox("Is Trigger##CapsuleColliderComponent ", &mIsTrigger);
		ImGui::DragFloat3("Center##CapsuleColliderComponent ", &mCenter.x, 0.1f);
		if (ImGui::DragFloat("Radius##BoxColliderComponent", &mRadius, 0.1f)) { SetRadius(mRadius); }
		if (ImGui::DragFloat("Height##BoxColliderComponent", &mHeight, 0.1f)) { SetHeight(mHeight); }
	}

	// TODO: Should use physics object pos/rot instead.
	if (mDebugFill)
	{
		SimpleDraw::AddFilledCapsule(GetCenter(), 32, 16, mRadius, mHeight, mTransformComponent->GetRotation(), mDebugColor);
	}
	else
	{
		SimpleDraw::AddCapsule(GetCenter(), 32, 16, mRadius, mHeight, mTransformComponent->GetRotation(), mDebugColor);
	}
}

void CapsuleColliderComponent::OnEnable()
{
	BaseColliderComponent::OnEnable();

	auto collider = std::make_unique<BoundingCapsule>(GetCenter(), mRadius, mHeight);
	mPhysicsObject = mPhysicsService->GetPhysicsWorld().CreatePhysicsObject(
		std::move(collider),
		mRigidBodyComponent ? (mRigidBodyComponent->IsKinematic() ? PhysicsObjectType::Kinematic : PhysicsObjectType::Dynamic) : PhysicsObjectType::Static);

	UpdatePhysicsObjectPropertys();
}

void CapsuleColliderComponent::SetRadius(float radius) 
{ 
	mRadius = Max(0.1f, radius);
}

void CapsuleColliderComponent::SetHeight(float height) 
{ 
	mHeight = Max(0.1f, height);
}
