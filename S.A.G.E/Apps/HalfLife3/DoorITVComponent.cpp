#include "SAGE/Inc/Precompiled.h"
#include "DoorITVComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"

using namespace SAGE;
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

	GameObject& owner = GetOwner();
	GameWorld& world = owner.GetWorld();
	for (const GameObjectHandle& handle : GetOwner().GetChildrenHandles())
	{
		if (GameObject* childObj = world.GetGameObject(handle))
		{
			if (childObj->GetName() == "Door")
			{
				mDoorObject = childObj;
			}
		}
	}

	return mDoorObject;
}