#include "SAGE/Inc/Precompiled.h"
#include "FlashlightComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/CameraService.h"
#include "SAGE/Inc/SpotlightComponent.h"
#include "SAGE/Inc/TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(FlashlightComponent, 1);

void FlashlightComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	
}

void FlashlightComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{

}

void FlashlightComponent::Initialize()
{
	GameObject& owner = GetOwner();
	GameWorld& world = owner.GetWorld();

	mCameraService = world.GetService<CameraService>();
	mTransformComponent = owner.GetComponent<TransformComponent>();
	mSpotlightComponent = owner.GetComponent<SpotlightComponent>();

	TurnOffFlashlight(); // TODO: should have a way to start game objects off via json.
}

void FlashlightComponent::Terminate()
{
	mSpotlightComponent = nullptr;
	mCameraService = nullptr;
}

void FlashlightComponent::Update(float deltaTime)
{
	Camera& camera = mCameraService->GetCamera();
	mTransformComponent->SetPosition(camera.GetPosition());
	mTransformComponent->SetRotation(camera.GetOrientation());
	mSpotlightComponent->Invalidate();
}

void FlashlightComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Flashlight Component##FlashlightComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::Button("Toggle Light##FlashlightComponent")) { ToggleFlashlight(); }
		if (ImGui::Button("Turn On##FlashlightComponent")) { TurnOnFlashlight(); }
		if (ImGui::Button("Turn Off##FlashlightComponent")) { TurnOffFlashlight(); }
	}
}

void FlashlightComponent::ToggleFlashlight()
{
	const GameObject& owner = GetOwner();

	if (owner.IsSelfActive())
	{
		TurnOffFlashlight();
	}
	else
	{
		TurnOnFlashlight();
	}
}

void FlashlightComponent::TurnOnFlashlight()
{
	GetOwner().SetActive(true);
}

void FlashlightComponent::TurnOffFlashlight()
{
	GetOwner().SetActive(false);
}
