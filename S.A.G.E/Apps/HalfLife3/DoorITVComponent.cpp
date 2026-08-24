#include "SAGE/Inc/Precompiled.h"
#include "DoorITVComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"

using namespace SAGE;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(DoorITVComponent, 100);

void DoorITVComponent::Initialize()
{
	InteractTriggerVolumeComponent::Initialize();
}

void DoorITVComponent::Terminate()
{
	mDoorObject = nullptr;
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
	if (GameObject* doorObj = GetDoorObject())
	{
		doorObj->SetActive(!doorObj->IsSelfActive());
	}
}

GameObject* DoorITVComponent::GetDoorObject()
{
	if (mDoorObject != nullptr)
	{
		return mDoorObject;
	}

	mDoorObject = GetOwner().FindChildByName("Door");
	return mDoorObject;
}