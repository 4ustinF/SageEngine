#include "Precompiled.h"
#include "CameraUtil.h"

using namespace SAGE::Input;

void SAGE::CameraUtil::UpdateFPSCamera(SAGE::Graphics::Camera& camera, float normSpeed, float fastSpeed, float turnSpeed, float deltaTime, bool useDebugMode)
{
	auto inputSystem = InputSystem::Get();
	const float moveSpeed = inputSystem->IsKeyDown(KeyCode::LSHIFT) ? fastSpeed : normSpeed;

	if (inputSystem->IsKeyDown(KeyCode::W))
		camera.Walk(moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::S))
		camera.Walk(-moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::D))
		camera.Strafe(moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::A))
		camera.Strafe(-moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::Q))
		camera.Rise(moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::E))
		camera.Rise(-moveSpeed * deltaTime);
	
	if (useDebugMode)
	{
		if (inputSystem->IsMouseDown(MouseButton::RBUTTON)) {
			camera.Yaw(inputSystem->GetMouseMoveX() * turnSpeed * deltaTime);
			camera.Pitch(inputSystem->GetMouseMoveY() * turnSpeed * deltaTime);	
		}
	}
	else
	{
		camera.Yaw(inputSystem->GetMouseMoveX()	* turnSpeed * deltaTime);
		camera.Pitch(inputSystem->GetMouseMoveY() * turnSpeed * 0.7f * deltaTime);
		inputSystem->CenterMouse();
	}
}