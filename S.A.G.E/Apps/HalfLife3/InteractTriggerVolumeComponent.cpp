#include "SAGE/Inc/Precompiled.h"
#include "InteractVolumeComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(InteractVolumeComponent, 100);

void InteractVolumeComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	// TODO: 
}

void InteractVolumeComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	// TODO:
}

void InteractVolumeComponent::Initialize()
{
	mInputSystem = InputSystem::Get();
}

void InteractVolumeComponent::Terminate()
{
	mInputSystem = nullptr;
}

void InteractVolumeComponent::OnTriggerStay(Collider* collider)
{
	if (mCanInteract && mInputSystem->IsKeyPressed(KeyCode::E))
	{
		OnInteract();
	}
}