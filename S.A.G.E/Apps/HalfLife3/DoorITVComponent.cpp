#include "SAGE/Inc/Precompiled.h"
#include "DoorITVComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/RBPhysicsService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(DoorITVComponent, 100);

void DoorITVComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	InteractTriggerVolumeComponent::LoadComponentFromTemplate(value);
	// TODO:
}

void DoorITVComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	InteractTriggerVolumeComponent::SaveComponentToTemplate(compObj, allocator);
	// TODO:
}

void DoorITVComponent::Initialize()
{
	InteractTriggerVolumeComponent::Initialize();
}

void DoorITVComponent::Terminate()
{
	InteractTriggerVolumeComponent::Terminate();
}

void DoorITVComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Door ITV Component##DoorITVComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{

	}
}

void DoorITVComponent::OnInteract()
{
	GetOwner().SetActive(false);
}
