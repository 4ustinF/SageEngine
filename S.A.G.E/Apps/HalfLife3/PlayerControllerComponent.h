#pragma once

#include "TypeIds.h"

class SAGE::Graphics::Camera;
class SAGE::Input::InputSystem;
class SAGE::CameraService;
class SAGE::CapsuleColliderComponent;
class FlashlightComponent;

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

private:
	SAGE::CameraService* mCameraService = nullptr;
	SAGE::RBPhysicsService* mRBPhysicsService = nullptr;
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::CapsuleColliderComponent* mCapsuleColliderComponent = nullptr;
	FlashlightComponent* mFlashlightComponent = nullptr;
	FlashlightComponent* LazyGetFlashlightComponent(); // Figure out a better way to get a childs comp.

	void IsGroundedCheck();
	void CheckForFlashlightInput();
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
	SAGE::Math::Vector3 mCameraOffset = SAGE::Math::Vector3(0.0f, -0.1f, 0.0f);

};