#include "Precompiled.h"
#include "FPSCameraControllerComponent.h"

#include "CameraComponent.h"
#include "GameObject.h"

#include "CameraUtil.h"

using namespace SAGE;
using namespace SAGE::Input;

MEMORY_POOL_DEFINE(FPSCameraControllerComponent, 80);

void FPSCameraControllerComponent::Initialize()
{
	mCameraComponent = GetOwner().GetComponent<CameraComponent>();
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
		ImGui::InputFloat("Normal Speed##FPSCameraControllerComponent", &normSpeed, 0.1f, 0.05f);
		ImGui::InputFloat("Fast Speed##FPSCameraControllerComponent", &fastSpeed, 0.1f, 0.05f);
		ImGui::InputFloat("Turn Speed##FPSCameraControllerComponent", &turnSpeed, 0.01f, 0.05f);
	}
}