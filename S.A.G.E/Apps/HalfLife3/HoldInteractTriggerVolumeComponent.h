#pragma once

#include "TypeIds.h"

class HoldInteractTriggerVolumeComponent : public SAGE::Component
{
public:
	void Initialize() override;
	void Terminate() override;

	void OnQueueUpdate(float deltaTime) override;

	void OnTriggerEnter(SAGE::RBPhysics::Collider* collider) override;
	void OnTriggerExit(SAGE::RBPhysics::Collider* collider) override;

protected:
	virtual void OnInteract(float deltaTime) {};

	SAGE::Input::InputSystem* mInputSystem = nullptr;
	bool mCanTrigger = true;
};