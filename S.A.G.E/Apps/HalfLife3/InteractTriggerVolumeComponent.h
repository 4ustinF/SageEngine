#pragma once

#include "TypeIds.h"

class InteractTriggerVolumeComponent : public SAGE::Component
{
public:
	void Initialize() override;
	void Terminate() override;

	void OnTriggerStay(SAGE::RBPhysics::Collider* collider) override;

protected:
	virtual void OnInteract() {};

	SAGE::Input::InputSystem* mInputSystem = nullptr;

	bool mIsOneShot = false;
	bool mTriggered = false;
};