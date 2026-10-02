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
	SAGE::Graphics::SoundEffectManager* mSoundEffectManager = nullptr;
	SAGE::GameObject* mDoorGameObj = nullptr;
	SAGE::TransformComponent* mDoorTransformComp = nullptr;

	bool mIsDoorLocked = false;
	bool mIsOpening = false;
	bool mIsAnimating = false;
	float mElpasedTime = 0.0f;
	float mAnimationTime = 0.5f;
	float mDoorEndPos = -1.22f;

	SAGE::Graphics::SoundId mOnLockedDoorInteractedSoundID = 0;
	float mPitch = 0.01f;
};