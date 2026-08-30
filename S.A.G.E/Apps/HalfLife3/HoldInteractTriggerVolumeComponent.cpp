#include "SAGE/Inc/Precompiled.h"
#include "HoldInteractTriggerVolumeComponent.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

void HoldInteractTriggerVolumeComponent::Initialize()
{
	mInputSystem = InputSystem::Get();
}

void HoldInteractTriggerVolumeComponent::Terminate()
{
	mInputSystem = nullptr;
}

void HoldInteractTriggerVolumeComponent::OnQueueUpdate(float deltaTime)
{
	if (mInputSystem->IsKeyDown(KeyCode::E)) // TODO: Set this value via imgui?
	{
		OnInteract(deltaTime);
	}

	if (mCanTrigger)
	{
		EnqueueUpdate();
	}
}

void HoldInteractTriggerVolumeComponent::OnTriggerEnter(Collider* collider)
{
	if (mCanTrigger)
	{
		EnqueueUpdate();
	}
}

void HoldInteractTriggerVolumeComponent::OnTriggerExit(Collider* collider)
{
	ClearQueueUpdate();
}