#pragma once
#include "HoldInteractTriggerVolumeComponent.h"

class HealthChargerHITVComponent final : public HoldInteractTriggerVolumeComponent
{
public:
	SET_TYPE_ID(ComponentId::HealthChargerHITV);
	MEMORY_POOL_DECLARE;
	
	const char* GetCompName() override { return "Health Charger HITV Component"; }

	void Initialize() override;
	void Terminate() override;
	void DebugUI() override;

	void OnEnable() override;
	
protected:
	void OnInteractStart() override;
	void OnInteract(float deltaTime) override;
	void OnInteractEnd() override;
	void OnEmptiedCharger();
	void ResetCharger();

	SAGE::MeshRendererComponent* GetFrontMedkitMeshRenderer();
	SAGE::MeshRendererComponent* mFrontMedkitMeshRenderer = nullptr;
	SAGE::Graphics::SoundEffectManager* mSoundEffectManager = nullptr;

	int mChargeAmount = 50; // charger's remaining charge
	int mMaxChargeAmount = 50; // charger's max charge
	float mDrainRate = 10.0f; // units per second
	float mDrainAccumulator = 0.0f;

	SAGE::Graphics::SoundId mMedShot4SoundID = 0;
	SAGE::Graphics::SoundId mMedCharge4SoundID = 0;
	SAGE::Graphics::SoundId mMedShotNo1SoundID = 0;
	float mAudioVolume = 0.1f; // TODO: We need a better game play system wise on how to handle audio levels.
};