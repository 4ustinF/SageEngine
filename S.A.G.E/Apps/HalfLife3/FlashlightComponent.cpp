#include "SAGE/Inc/Precompiled.h"
#include "FlashlightComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/CameraService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
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

}

void FlashlightComponent::Terminate()
{

}

void FlashlightComponent::Update(float deltaTime)
{

}

void FlashlightComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Flashlight Component##FlashlightComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{

	}
}
