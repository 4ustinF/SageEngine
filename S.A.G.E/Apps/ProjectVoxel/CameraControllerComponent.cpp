#include "CameraControllerComponent.h"

#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/CameraUtil.h"
#include "SAGE/Inc/CameraComponent.h"
#include "SAGE/Inc/TransformComponent.h"

#include "PlayerUIService.h"
#include "PlayerControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Input;

MEMORY_POOL_DEFINE(CameraControllerComponent, 100);

void CameraControllerComponent::Initialize()
{
	mCameraComponent = GetOwner().GetComponent<CameraComponent>();
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	mPlayerUIService = GetOwner().GetWorld().GetService<PlayerUIService>();
	mPlayerControllerComponent = GetOwner().GetComponent<PlayerControllerComponent>();

	InputSystem::Get()->ShowSystemCursor(false);
}

void CameraControllerComponent::Terminate()
{
	mPlayerControllerComponent = nullptr;
	mPlayerUIService = nullptr;
	mTransformComponent = nullptr;
	mCameraComponent = nullptr;
}

void CameraControllerComponent::Update(float deltaTime)
{
	const auto& inputSystem = InputSystem::Get();

	if (mPlayerUIService->IsUIShowing())
	{
		return;
	}

	CameraUtil::UpdateFPSCamera(mCameraComponent->GetCamera(), 0.0f, 0.0f, turnSpeed, deltaTime, false);
	return;

	//if (inputSystem->IsKeyPressed(mCycleKeyCode))
	//{
	//	mFirstPerson = !mFirstPerson;
	//}

	//if (mFirstPerson)
	//{
	//	if (mPlayerControllerComponent->IsSurvivalMode())
	//	{
	//	}
	//	else
	//	{
	//		CameraUtil::UpdateFPSCamera(mCameraComponent->GetCamera(), normSpeed, fastSpeed, turnSpeed, deltaTime, false);
	//	}
	//}
	//else
	//{
	//	// Rotation
	//	if (inputSystem->IsMouseDown(MouseButton::RBUTTON))
	//	{
	//		auto mouseX = inputSystem->GetMouseMoveX() * deltaTime;
	//		auto rotation = Math::Quaternion::RotationEuler({ 0.0f, mouseX, 0.0f });
	//		mTransformComponent->rotation = mTransformComponent->rotation * rotation;
	//	}

	//	const auto transform = mTransformComponent->GetMatrix4();
	//	const auto position = mTransformComponent->position;
	//	const auto distance = Math::GetLook(transform) * -mDistance;
	//	const auto cameraElevation = Math::Vector3::YAxis * mCameraElevation;
	//	const auto lookElevation = Math::Vector3::YAxis * mLookElevation;

	//	auto& camera = mCameraComponent->GetCamera();
	//	camera.SetPosition(position + distance + cameraElevation);
	//	camera.SetLookAt(position + lookElevation);
	//}
}

void CameraControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Camera Controller Component##CameraControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (mFirstPerson)
		{
			ImGui::InputFloat("Normal Speed##FPSCameraControllerComponent", &normSpeed, 0.1f, 0.05f);
			ImGui::InputFloat("Fast Speed##FPSCameraControllerComponent", &fastSpeed, 0.1f, 0.05f);
			ImGui::InputFloat("Turn Speed##FPSCameraControllerComponent", &turnSpeed, 0.01f, 0.05f);
		}
		else
		{
			ImGui::InputFloat("Distance##FPSCameraControllerComponent", &mDistance, 0.1f, 0.05f);
			ImGui::InputFloat("Camera Elevation##FPSCameraControllerComponent", &mCameraElevation, 0.1f, 0.05f);
			ImGui::InputFloat("Look Elevation##FPSCameraControllerComponent", &mLookElevation, 0.1f, 0.05f);
		}
	}
}