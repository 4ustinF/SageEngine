#pragma once

#include "TypeIds.h"
class SAGE::CameraComponent;
class SAGE::TransformComponent;
class PlayerUIService;
class PlayerControllerComponent;

class CameraControllerComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::VoxelCameraController);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void DebugUI() override;

	void SetNormSpeed(float speed) { normSpeed = speed; }
	void SetFastSpeed(float speed) { fastSpeed = speed; }
	void SetTurnSpeed(float speed) { turnSpeed = speed; }

	void SetDistance(float distance) { mDistance = distance; }
	void SetCameraElevation(float elevation) { mCameraElevation = elevation; }
	void SetLookElevation(float elevation) { mLookElevation = elevation; }

	void SetCycleKey(SAGE::Input::KeyCode keyCode) { mCycleKeyCode = keyCode; }

private:
	SAGE::CameraComponent* mCameraComponent = nullptr;
	SAGE::TransformComponent* mTransformComponent = nullptr;
	PlayerUIService* mPlayerUIService = nullptr;
	PlayerControllerComponent* mPlayerControllerComponent = nullptr;

	float normSpeed = 2.0f;
	float fastSpeed = 5.0f;
	float turnSpeed = 0.1f;

	float mDistance = 1.0f;
	float mCameraElevation = 1.0f;
	float mLookElevation = 1.0f;

	SAGE::Input::KeyCode mCycleKeyCode = SAGE::Input::KeyCode::F;

	bool mFirstPerson = true;
};
