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
		if (!mIsInteracting)
		{
			StartInteracting();
		}

		OnInteract(deltaTime);
	}
	else if (mIsInteracting)
	{
		StopInteracting();
	}

	if (mCanTrigger)
	{
		EnqueueUpdate();
	}
}

void HoldInteractTriggerVolumeComponent::OnTriggerEnter(Collider* collider)
{
	mIsOverlapping = true;

	if (mCanTrigger)
	{
		EnqueueUpdate();
	}
}

void HoldInteractTriggerVolumeComponent::OnTriggerExit(Collider* collider)
{
	mIsOverlapping = false;
	ClearQueueUpdate();

	if (mIsInteracting)
	{
		StopInteracting();
	}
}

void HoldInteractTriggerVolumeComponent::SetCanTrigger(bool canTrigger)
{
	if (mCanTrigger == canTrigger)
	{
		return;
	}

	mCanTrigger = canTrigger;

	if (mCanTrigger)
	{
		if (mIsOverlapping)
		{
			EnqueueUpdate();
		}
	}
	else if (mIsInteracting)
	{
		ClearQueueUpdate();
		StopInteracting();
	}
}

void HoldInteractTriggerVolumeComponent::StartInteracting()
{
	mIsInteracting = true;
	OnInteractStart();
}

void HoldInteractTriggerVolumeComponent::StopInteracting()
{
	mIsInteracting = false;
	OnInteractEnd();
}