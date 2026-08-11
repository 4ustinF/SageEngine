#include "PlayerControllerComponent.h"

#include "SAGE/Inc/CameraService.h"
#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/CapsuleColliderComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(PlayerControllerComponent, 100);

void PlayerControllerComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	
}

void PlayerControllerComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{

}

void PlayerControllerComponent::Initialize()
{
	mInputSystem = InputSystem::Get();

	GameObject& owner = GetOwner();
	mCameraService = owner.GetWorld().GetService<CameraService>();
	mCapsuleColliderComponent = owner.GetComponent<CapsuleColliderComponent>();
}

void PlayerControllerComponent::Terminate()
{
	mCameraService = nullptr;
	mInputSystem = nullptr;
	mCapsuleColliderComponent = nullptr;
}

void PlayerControllerComponent::Update(float deltaTime)
{
	Camera& camera = mCameraService->GetCamera();
	IsGroundedCheck();
	CheckForPlayerMovementInput(camera, deltaTime);
	UpdateCameraPosition(camera);
}

void PlayerControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Controller Component##PlayerControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Checkbox("Can Move##PlayerControllerComponent", &mCanMove);
		ImGui::Checkbox("Is FPS##PlayerControllerComponent", &mIsInFPSMode);
		ImGui::Checkbox("Is Grounded##PlayerControllerComponent", &mIsGrounded);
		ImGui::DragFloat2("Ground Speed##PlayerControllerComponent", &mGroundSpeed.x, 0.1f);
		ImGui::DragFloat2("Air Speed##PlayerControllerComponent", &mAirSpeed.x, 0.1f);
		ImGui::DragFloat("Jump Force##PlayerControllerComponent", &mJumpForce, 0.1f);
	}
}

void PlayerControllerComponent::IsGroundedCheck()
{
	// TODO: Implement grounded check using raycast or collision detection with the ground.
	mIsGrounded = true; // Placeholder for actual grounded check logic.
}

void PlayerControllerComponent::CheckForPlayerMovementInput(Camera& camera, float deltaTime)
{
	if (!mCanMove)
	{
		return;
	}

	RBPhysicsObject* physicsObject = mCapsuleColliderComponent->GetPhysicsObject();
	if (physicsObject == nullptr)
	{
		return;
	}

	const float moveSpeed = GetMovementSpeed(deltaTime);

	// 1. Get camera forward, flatten to XZ plane (ignore pitch)
	const Vector3 forward = camera.GetDirectionWithoutPitch();

	// 2. Right vector = forward rotated 90 degrees around Y (cross of world-up and forward gives you a perpendicular horizontal vector)
	const Vector3 right = Normalize(Cross(Vector3::YAxis, forward));

	if (mInputSystem->IsKeyDown(KeyCode::UP))
	{
		physicsObject->ApplyForce(forward * moveSpeed);
	}

	if (mInputSystem->IsKeyDown(KeyCode::DOWN))
	{
		physicsObject->ApplyForce(-forward * moveSpeed);
	}

	if (mInputSystem->IsKeyDown(KeyCode::RIGHT))
	{
		physicsObject->ApplyForce(right * moveSpeed);
	}

	if (mInputSystem->IsKeyDown(KeyCode::LEFT))
	{
		physicsObject->ApplyForce(-right * moveSpeed);
	}

	if (mInputSystem->IsKeyPressed(KeyCode::NUMPAD0))
	{
		physicsObject->ApplyForce(Vector3::YAxis * mJumpForce * deltaTime);
	}
}

void PlayerControllerComponent::UpdateCameraPosition(Camera& camera)
{
	if (!mIsInFPSMode) // TODO: Also check if we are in edit mode
	{
		return;
	}

	RBPhysicsObject* physicsObject = mCapsuleColliderComponent->GetPhysicsObject();
	if (physicsObject == nullptr)
	{
		return;
	}

	const Vector3 camNewPos = ((BoundingCapsule&)physicsObject->GetCollider()).GetInnerTopCenter();
	camera.SetPosition(camNewPos + Vector3(0.0f, 0.5f, 0.0f));
}

float PlayerControllerComponent::GetMovementSpeed(float deltaTime) const
{
	if (mInputSystem->IsKeyDown(KeyCode::LSHIFT))
	{
		return (mIsGrounded ? mGroundSpeed.y : mAirSpeed.y) * deltaTime;
	}

	return (mIsGrounded ? mGroundSpeed.x : mAirSpeed.x) * deltaTime;
}