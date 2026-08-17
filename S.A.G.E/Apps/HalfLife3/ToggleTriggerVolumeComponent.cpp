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
	
}

void ToggleTriggerVolumeComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{

}

void ToggleTriggerVolumeComponent::Initialize()
{

}

void ToggleTriggerVolumeComponent::Terminate()
{

}

void ToggleTriggerVolumeComponent::Update(float deltaTime)
{

}

void ToggleTriggerVolumeComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Toggle Trigger Volume Component##ToggleTriggerVolumeComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{

	}
}

void ToggleTriggerVolumeComponent::OnTriggerEnter(Collider* collider)
{
}

void ToggleTriggerVolumeComponent::OnTriggerStay(Collider* collider)
{
}

void ToggleTriggerVolumeComponent::OnTriggerExit(Collider* collider)
{

}
