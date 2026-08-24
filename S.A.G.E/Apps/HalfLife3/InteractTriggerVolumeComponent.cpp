#include "SAGE/Inc/Precompiled.h"
#include "InteractTriggerVolumeComponent.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

void InteractTriggerVolumeComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	// TODO:
}

void InteractTriggerVolumeComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	// TODO:
}

void InteractTriggerVolumeComponent::Initialize()
{
	mInputSystem = InputSystem::Get();
}

void InteractTriggerVolumeComponent::Terminate()
{
	mInputSystem = nullptr;
}

void InteractTriggerVolumeComponent::OnTriggerStay(Collider* collider)
{
	if (mIsOneShot)
	{
		if (mTriggered)
		{
			return;
		}

		mTriggered = true;
	}

	if (mInputSystem->IsKeyPressed(KeyCode::E)) // TODO: Set this value via imgui?
	{
		OnInteract();
	}
}