#include "SAGE/Inc/Precompiled.h"
#include "PlayerControllerComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/CameraService.h"
#include "SAGE/Inc/RBPhysicsService.h"
#include "SAGE/Inc/AnimatorComponent.h"
#include "SAGE/Inc/CapsuleColliderComponent.h"
#include "FlashlightComponent.h"
#include "PlayerWeaponComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(PlayerControllerComponent, 1);

void PlayerControllerComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("Time To Crouch"))
	{
		mTimeToCrouch = value["Time To Crouch"].GetFloat();
	}

	if (value.HasMember("Crouch Height"))
	{
		mCrouchHeight = value["Crouch Height"].GetFloat();
	}
}

void PlayerControllerComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	SaveNumberToTemplate(compObj, allocator, "Time To Crouch", mTimeToCrouch);
	SaveNumberToTemplate(compObj, allocator, "Crouch Height", mCrouchHeight);
}

void PlayerControllerComponent::Initialize()
{
	mInputSystem = InputSystem::Get();

	GameObject& owner = GetOwner();
	GameWorld& world = owner.GetWorld();
	mCameraService = world.GetService<CameraService>();
	mRBPhysicsService = world.GetService<RBPhysicsService>();
	mCapsuleColliderComponent = owner.GetComponent<CapsuleColliderComponent>();

	mFlashlightComponent = owner.GetComponentInChildren<FlashlightComponent>();
	mWeapons = owner.GetComponentsInChildren<PlayerWeaponComponent>(); 
	mActiveWeapon = mWeapons.empty() ? nullptr : mWeapons[mActiveWeaponIndex];

	mStandHeight = mCapsuleColliderComponent->GetHeight();
}

void PlayerControllerComponent::Terminate()
{
	mCameraService = nullptr;
	mRBPhysicsService = nullptr;
	mInputSystem = nullptr;
	mCapsuleColliderComponent = nullptr;
	mFlashlightComponent = nullptr;
}

void PlayerControllerComponent::Update(float deltaTime)
{
	Camera& camera = mCameraService->GetCamera();
	IsGroundedCheck();
	CheckForFlashlightInput();
	CheckForPlayerCrouchingInput(deltaTime);
	CheckForPlayerMovementInput(camera, deltaTime);
	CheckForPlayerGameplayInput();
	UpdateCameraPosition(camera);
	UpdateArmsPosition(camera);
}

void PlayerControllerComponent::OnQueueUpdate(float deltaTime)
{
	if (!mIsCrouching) // Attempt to stand
	{
		if (mCrouchAmount > 0.0f)
		{
			if (const RBPhysicsObject* physicsObject = mCapsuleColliderComponent->GetPhysicsObject())
			{
				if (const BoundingCapsule* boundingCapsule = dynamic_cast<const BoundingCapsule*>(physicsObject->GetCollider()))
				{
					const Vector3 rayOrigin = boundingCapsule->GetTopCenter() + (Vector3::YAxis * Constants::Epsilon);
					if (!mRBPhysicsService->GetPhysicsWorld().Raycast(rayOrigin, Vector3::YAxis, 0.2f))
					{
						mCrouchElapsedTime = Max(mCrouchElapsedTime - deltaTime, 0.0f);
						mCrouchAmount = mCrouchElapsedTime / mTimeToCrouch;
						const float newCapsuleHeight = Lerp(mStandHeight, mCrouchHeight, mCrouchAmount);
						mCapsuleColliderComponent->SetHeight(newCapsuleHeight);
					}
				}
			}

			EnqueueUpdate();
		}
	}
}

void PlayerControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Controller Component##PlayerControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::Text("General Data");
		ImGui::Checkbox("Can Move##PlayerControllerComponent", &mCanMove);
		ImGui::Checkbox("Is FPS##PlayerControllerComponent", &mIsInFPSMode);
		ImGui::Checkbox("Is Grounded##PlayerControllerComponent", &mIsGrounded);
		ImGui::DragFloat("Jump Force##PlayerControllerComponent", &mJumpForce, 0.1f);
		ImGui::DragFloat3("Camera Offset##PlayerControllerComponent", &mCameraOffset.x, 0.01f);

		ImGui::Text("Movement Data");
		ImGui::DragFloat2("Ground Speed##PlayerControllerComponent", &mGroundSpeed.x, 0.1f);
		ImGui::DragFloat2("Air Speed##PlayerControllerComponent", &mAirSpeed.x, 0.1f);

		ImGui::Text("Crouch Data");
		ImGui::Checkbox("Is Crouching##PlayerControllerComponent", &mIsCrouching);
		ImGui::DragFloat("Time To Crouch##PlayerControllerComponent", &mTimeToCrouch, 0.1f, 0.01f);

		ImGui::Text("Weapon Data");
		ImGui::DragInt("Active Weapon Index##PlayerControllerComponent", &mActiveWeaponIndex, 1, 0, (int)mWeapons.size() - 1);
	}
}

void PlayerControllerComponent::IsGroundedCheck()
{
	if (const RBPhysicsObject* physicsObject = mCapsuleColliderComponent->GetPhysicsObject())
	{
		if (const BoundingCapsule* boundingCapsule = dynamic_cast<const BoundingCapsule*>(physicsObject->GetCollider()))
		{
			PhysicsRayHit hit;
			const Vector3 rayOrigin = boundingCapsule->GetBottomCenter() - (Vector3::YAxis * Constants::Epsilon);
			mIsGrounded = mRBPhysicsService->GetPhysicsWorld().Raycast(rayOrigin, -Vector3::YAxis, 0.01f, hit);

			if (mIsGrounded)
			{
				mGroundNormal = hit.normal;
			}
			return;
		}
	}

	mIsGrounded = false;
}

void PlayerControllerComponent::CheckForFlashlightInput()
{
	if (mInputSystem->IsKeyPressed(KeyCode::F))
	{
		if (mFlashlightComponent != nullptr)
		{
			mFlashlightComponent->ToggleFlashlight();
		}
	}
}

void PlayerControllerComponent::CheckForPlayerCrouchingInput(float deltaTime)
{
	if (mInputSystem->IsKeyDown(KeyCode::LCONTROL))
	{
		if (mIsCrouching)
		{
			const float currentCapsuleHeight = mCapsuleColliderComponent->GetHeight();
			if (mCrouchAmount < 1.0f)
			{
				mCrouchElapsedTime = Min(mCrouchElapsedTime + deltaTime, mTimeToCrouch);
				mCrouchAmount = mCrouchElapsedTime / mTimeToCrouch;
				const float newCapsuleHeight = Lerp(mStandHeight, mCrouchHeight, mCrouchAmount);
				mCapsuleColliderComponent->SetHeight(newCapsuleHeight);
			}

			return;
		}

		mIsCrouching = true;
	}
	else if (mInputSystem->IsKeyReleased(KeyCode::LCONTROL))
	{
		mIsCrouching = false;
		EnqueueUpdate();
	}
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
	Vector3 forward = camera.GetDirectionWithoutPitch();

	// 2. Right vector = forward rotated 90 degrees around Y (cross of world-up and forward gives you a perpendicular horizontal vector)
	Vector3 right = Normalize(Cross(Vector3::YAxis, forward));

	// 3. If grounded, bend forward/right to follow the surface instead of staying purely horizontal
	if (mIsGrounded)
	{
		forward = Normalize(ProjectOnPlane(forward, mGroundNormal));
		right = Normalize(ProjectOnPlane(right, mGroundNormal));
	}

	if (mInputSystem->IsKeyDown(mIsInFPSMode ? KeyCode::W : KeyCode::UP))
	{
		physicsObject->ApplyForce(forward * moveSpeed);
	}

	if (mInputSystem->IsKeyDown(mIsInFPSMode ? KeyCode::S : KeyCode::DOWN))
	{
		physicsObject->ApplyForce(-forward * moveSpeed);
	}

	if (mInputSystem->IsKeyDown(mIsInFPSMode ? KeyCode::D : KeyCode::RIGHT))
	{
		physicsObject->ApplyForce(right * moveSpeed);
	}

	if (mInputSystem->IsKeyDown(mIsInFPSMode ? KeyCode::A : KeyCode::LEFT))
	{
		physicsObject->ApplyForce(-right * moveSpeed);
	}

	if (mIsGrounded)
	{
		if (mInputSystem->IsKeyPressed(mIsInFPSMode ? KeyCode::SPACE : KeyCode::NUMPAD0))
		{
			physicsObject->ApplyForce(Vector3::YAxis * mJumpForce * deltaTime);
			mIsGrounded = false;
		}
	}
}

void PlayerControllerComponent::CheckForPlayerGameplayInput()
{
	if (!mIsInFPSMode) // TODO: Also check if we are in edit mode
	{
		return;
	}

	if (mInputSystem->IsScrollWheelUp())
	{
		SwapWeapons(mActiveWeaponIndex - 1);
	}
	else if (mInputSystem->IsScrollWheelDown())
	{
		SwapWeapons(mActiveWeaponIndex + 1);
	}

	if (mActiveWeapon)
	{
		mActiveWeapon->CheckForPlayerGameplayInput();
	}
}

void PlayerControllerComponent::UpdateCameraPosition(Camera& camera)
{
	if (!mIsInFPSMode)
	{
		return;
	}

	RBPhysicsObject* physicsObject = mCapsuleColliderComponent->GetPhysicsObject();
	if (physicsObject == nullptr)
	{
		return;
	}

	if (const BoundingCapsule* boundingCapsule = dynamic_cast<BoundingCapsule*>(physicsObject->GetCollider()))
	{
		const Vector3 camNewPos = boundingCapsule->GetTopCenter() + mCameraOffset;
		camera.SetPosition(camNewPos);
	}
}

void PlayerControllerComponent::UpdateArmsPosition(const Camera& camera)
{
	if (!mIsInFPSMode) // TODO: Also check if we are in edit mode
	{
		return;
	}

	if (mActiveWeapon != nullptr)
	{
		mActiveWeapon->UpdateWeaponRotationAndPosition(camera);
	}
}

float PlayerControllerComponent::GetMovementSpeed(float deltaTime) const
{
	if (mInputSystem->IsKeyDown(KeyCode::LSHIFT))
	{
		return (mIsGrounded ? mGroundSpeed.y : mAirSpeed.y) * deltaTime;
	}

	return (mIsGrounded ? mGroundSpeed.x : mAirSpeed.x) * deltaTime;
}

void PlayerControllerComponent::SwapWeapons(int mNewIndex)
{
	// Weapon swapping = Active Disappears, new weapon appears with Draw animation.
	if (mActiveWeapon)
	{
		if (mActiveWeapon->IsBusy())
		{
			return;
		}

		mActiveWeapon->GetOwner().SetActive(false);
	}

	const int weaponCount = static_cast<int>(mWeapons.size());
	mActiveWeaponIndex = WrapInclusive(mNewIndex, 0, weaponCount);

	if (mActiveWeaponIndex == weaponCount)
	{
		mActiveWeapon = nullptr;
	}
	else if (mActiveWeapon = mWeapons[mActiveWeaponIndex])
	{
		mActiveWeapon->GetOwner().SetActive(true);
	}
}