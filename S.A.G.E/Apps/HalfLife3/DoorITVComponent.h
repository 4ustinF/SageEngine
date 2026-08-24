#pragma once
#include "InteractTriggerVolumeComponent.h"

class DoorITVComponent final : public InteractTriggerVolumeComponent
{
public:
	SET_TYPE_ID(ComponentId::DoorITV);
	MEMORY_POOL_DECLARE;
	
	const char* GetCompName() override { return "Door ITV Component"; }

	void Initialize() override;
	void Terminate() override;

	void OnQueueUpdate(float deltaTime) override;
	void DebugUI() override;
	
protected:
	void OnInteract() override;

private:
	SAGE::TransformComponent* GetDoorTransformComp();
	SAGE::GameObject* mDoorGameObj = nullptr;
	SAGE::TransformComponent* mDoorTransformComp = nullptr;

	bool mIsOpening = false;
	bool mIsAnimating = false;
	float mElpasedTime = 0.0f;
	float mAnimationTime = 0.5f;
	float mDoorEndPos = -1.22f;
};