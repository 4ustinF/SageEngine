#include "SAGE/Inc/Precompiled.h"
#include "FlashlightComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/CameraService.h"
#include "SAGE/Inc/SpotlightComponent.h"
#include "SAGE/Inc/TransformComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
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
	mInputSystem = InputSystem::Get();

	GameObject& owner = GetOwner();
	GameWorld& world = GetOwner().GetWorld();

	mCameraService = world.GetService<CameraService>();
	mTransformComponent = owner.GetComponent<TransformComponent>();
	mSpotlightComponent = owner.GetComponent<SpotlightComponent>();
}

void FlashlightComponent::Terminate()
{
	mSpotlightComponent = nullptr;
	mCameraService = nullptr;
	mInputSystem = nullptr;
}

void FlashlightComponent::Update(float deltaTime)
{
	Camera& camera = mCameraService->GetCamera();
	//mTransformComponent->SetRotation(camera.GetOrientation());
	mSpotlightComponent->Invalidate();
}

void FlashlightComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Flashlight Component##FlashlightComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{

	}
}
