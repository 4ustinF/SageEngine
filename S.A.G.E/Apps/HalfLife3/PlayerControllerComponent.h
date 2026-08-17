#pragma once

#include "TypeIds.h"

class SAGE::CameraService;
class SAGE::CapsuleColliderComponent;
class SAGE::Input::InputSystem;
class SAGE::Graphics::Camera;

class PlayerControllerComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PlayerController);
	MEMORY_POOL_DECLARE;

	const char* GetCompName() override { return "Player Controller Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void OnTriggerEnter(SAGE::RBPhysics::Collider* collider) override;
	void OnTriggerStay(SAGE::RBPhysics::Collider* collider) override;
	void OnTriggerExit(SAGE::RBPhysics::Collider* collider) override;

private:
	SAGE::CameraService* mCameraService = nullptr;
	SAGE::RBPhysicsService* mRBPhysicsService = nullptr;
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::CapsuleColliderComponent* mCapsuleColliderComponent = nullptr;

	void IsGroundedCheck();
	void CheckForPlayerMovementInput(SAGE::Graphics::Camera& camera, float deltaTime);
	void UpdateCameraPosition(SAGE::Graphics::Camera& camera);

	float GetMovementSpeed(float deltaTime) const;

	SAGE::Math::Vector2 mGroundSpeed = SAGE::Math::Vector2(50.0f, 100.0f);
	SAGE::Math::Vector2 mAirSpeed = SAGE::Math::Vector2(5.0f, 10.0f);
	float mJumpForce = 500.0f;
	bool mIsGrounded = true;
	SAGE::Math::Vector3 mGroundNormal = SAGE::Math::Vector3::YAxis;

	// Debug
	bool mCanMove = true;
	bool mIsInFPSMode = false;

	bool mHasEntered = false;
	bool mIsStaying = false;
};