#include "Precompiled.h"
#include "CapsuleColliderComponent.h"

#include "CameraService.h"
#include "GameWorld.h"

#include "GameObject.h"
#include "RBPhysicsService.h"
#include "TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
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

void CapsuleColliderComponent::Update(float deltaTime)
{
	auto inputSystem = InputSystem::Get();
	const float moveSpeed = (inputSystem->IsKeyDown(KeyCode::LSHIFT) ? 100.0f : 50.0f) * deltaTime;

	CameraService* cameraService = GetOwner().GetWorld().GetService<CameraService>();
	Camera& camera = cameraService->GetCamera();

	// 1. Get camera forward, flatten to XZ plane (ignore pitch)
	Vector3 forward = camera.GetDirection();
	forward.y = 0.0f;
	forward = Normalize(forward);

	// 2. Right vector = forward rotated 90 degrees around Y
	//    (cross of world-up and forward gives you a perpendicular horizontal vector)
	Vector3 right = Cross(Vector3::YAxis, forward);
	right = Normalize(right);

	// 3. Apply forces relative to camera orientation
	if (inputSystem->IsKeyDown(KeyCode::UP))
		mPhysicsObject->ApplyForce(forward * moveSpeed);
	if (inputSystem->IsKeyDown(KeyCode::DOWN))
		mPhysicsObject->ApplyForce(-forward * moveSpeed);
	if (inputSystem->IsKeyDown(KeyCode::RIGHT))
		mPhysicsObject->ApplyForce(right * moveSpeed);
	if (inputSystem->IsKeyDown(KeyCode::LEFT))
		mPhysicsObject->ApplyForce(-right * moveSpeed);

	if (inputSystem->IsKeyDown(KeyCode::SPACE))
		mPhysicsObject->ApplyForce(Vector3::YAxis * moveSpeed * 0.5f);

	camera.SetPosition(mPhysicsObject->GetPosition() + Vector3(0.0f, 0.5f, 0.0f));
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
	mPhysicsObject = new RBPhysicsObject(new BoundingCapsule(GetCenter(), mRadius, mHeight));
	BaseColliderComponent::OnEnable();
}

void CapsuleColliderComponent::SetRadius(float radius) 
{ 
	mRadius = Max(0.1f, radius);
}

void CapsuleColliderComponent::SetHeight(float height) 
{ 
	mHeight = Max(0.1f, height);
}
