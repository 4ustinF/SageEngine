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

private:
	SAGE::CameraService* mCameraService = nullptr;
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::CapsuleColliderComponent* mCapsuleColliderComponent = nullptr;

	void CheckForPlayerMovementInput(SAGE::Graphics::Camera& camera, float deltaTime);
	void UpdateCameraPosition(SAGE::Graphics::Camera& camera);

	float mWalkSpeed = 50.0f;
	float mSprintSpeed = 100.0f;
	float mJumpForce = 500.0f;

	// Debug
	bool mCanMove = true;
	bool mIsInFPSMode = false;
};