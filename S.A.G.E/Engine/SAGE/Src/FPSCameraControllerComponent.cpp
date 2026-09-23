#include "Precompiled.h"
#include "FPSCameraControllerComponent.h"

#include "CameraComponent.h"
#include "GameObject.h"

#include "CameraUtil.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(FPSCameraControllerComponent, 10);

void FPSCameraControllerComponent::Initialize()
{
	mCameraComponent = GetOwner().GetComponent<CameraComponent>();
}

void FPSCameraControllerComponent::Terminate()
{
	mCameraComponent = nullptr;
}

void FPSCameraControllerComponent::Update(float deltaTime)
{
	const auto& inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(mDebugKeyCode))
	{
		useMouseDebugMode = !useMouseDebugMode;
		inputSystem->ShowSystemCursor(useMouseDebugMode);
	}

	CameraUtil::UpdateFPSCamera(mCameraComponent->GetCamera(), normSpeed, fastSpeed, turnSpeed, deltaTime, useMouseDebugMode);
}

void FPSCameraControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("FPS Camera Component##FPSCameraControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		Camera& camera = mCameraComponent->GetCamera();

		Vector3 position = camera.GetPosition();
		if (ImGui::DragFloat3("Position##FPSCameraControllerComponent", &position.x, 0.1f))
		{
			camera.SetPosition(position);	
		}

		Vector3 direction = camera.GetDirection();
		if (ImGui::DragFloat3("Direction##FPSCameraControllerComponent", &direction.x, 0.1f))
		{
			camera.SetDirection(direction);
		}

		float fov = camera.GetFOVInDegrees();
		if (ImGui::DragFloat("FOV##FPSCameraControllerComponent", &fov, 0.1f, camera.GetMinFOVInDegrees(), camera.GetMaxFOVInDegrees()))
		{
			camera.SetFovInDegrees(fov);
		}

		ImGui::InputFloat("Normal Speed##FPSCameraControllerComponent", &normSpeed, 0.1f, 0.05f);
		ImGui::InputFloat("Fast Speed##FPSCameraControllerComponent", &fastSpeed, 0.1f, 0.05f);
		ImGui::InputFloat("Turn Speed##FPSCameraControllerComponent", &turnSpeed, 0.01f, 0.05f);
	}
}
