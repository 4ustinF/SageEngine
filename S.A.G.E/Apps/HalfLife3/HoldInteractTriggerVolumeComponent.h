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

	bool GetCanTrigger() const { return mCanTrigger; }
	void SetCanTrigger(bool canTrigger);

protected:
	virtual void OnInteractStart() {};
	virtual void OnInteract(float deltaTime) {};
	virtual void OnInteractEnd() {};

	SAGE::Input::InputSystem* mInputSystem = nullptr;
	bool mIsInteracting = false;
	bool mCanTrigger = true;

private:
	void StartInteracting();
	void StopInteracting();
	bool mIsOverlapping = false;

};