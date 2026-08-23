#include "SAGE/Inc/Precompiled.h"
#include "InteractTriggerVolumeComponent.h"

#include "SAGE/Inc/GameWorld.h"
#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/RBPhysicsService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
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
	if (mIsOneShot && mTriggered)
	{
		return;
	}

	if (mInputSystem->IsKeyDown(KeyCode::E)) // TODO: Set this value via imgui?
	{
		OnInteract();

		if (mIsOneShot)
		{
			mTriggered = true;
		}
	}
}