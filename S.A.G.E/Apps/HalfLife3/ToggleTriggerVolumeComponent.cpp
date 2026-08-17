#include "SAGE/Inc/Precompiled.h"
#include "ToggleTriggerVolumeComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/RBPhysicsService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(ToggleTriggerVolumeComponent, 100);

void ToggleTriggerVolumeComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("Disable Name List"))
	{
		const auto& nameList = value["Disable Name List"].GetArray();
		mObjectsToDisableList.reserve(nameList.Size());
		for (const auto& name : nameList)
		{
			mObjectsToDisableList.push_back(name.GetString());
		}
	}

	if (value.HasMember("Enable Name List"))
	{
		const auto& nameList = value["Enable Name List"].GetArray();
		mObjectsToEnableList.reserve(nameList.Size());
		for (const auto& name : nameList)
		{
			mObjectsToEnableList.push_back(name.GetString());
		}
	}
}

void ToggleTriggerVolumeComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	// TODO:
}

void ToggleTriggerVolumeComponent::Terminate()
{
	mObjectsToDisableList.clear();
	mObjectsToEnableList.clear();
}

void ToggleTriggerVolumeComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Toggle Trigger Volume Component##ToggleTriggerVolumeComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		// TODO: Let us edit object names.

		char nameBuffer[128];
		ImGui::Text("Objects To Disable:");
		for (std::string& name : mObjectsToDisableList)
		{
			strncpy_s(nameBuffer, name.c_str(), sizeof(nameBuffer)); // copy std::string into buffer initially
			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
			{
				name = nameBuffer; // sync back to std::string when changed
			}
		}

		ImGui::Separator();
		ImGui::Text("Objects To Enable:");
		for (std::string& name : mObjectsToEnableList)
		{
			strncpy_s(nameBuffer, name.c_str(), sizeof(nameBuffer)); // copy std::string into buffer initially
			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
			{
				name = nameBuffer; // sync back to std::string when changed
			}
		}
	}
}

void ToggleTriggerVolumeComponent::OnTriggerEnter(Collider* collider)
{
	GameWorld& gameWorld = GetOwner().GetWorld();

	for (const std::string& name : mObjectsToDisableList)
	{
		if (GameObject* gameObject = gameWorld.FindGameObject(name))
		{
			gameObject->SetActive(false);
		}
	}

	for (const std::string& name : mObjectsToEnableList)
	{
		if (GameObject* gameObject = gameWorld.FindGameObject(name))
		{
			gameObject->SetActive(true);
		}
	}
}