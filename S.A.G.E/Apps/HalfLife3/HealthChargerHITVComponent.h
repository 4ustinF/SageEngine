#pragma once
#include "HoldInteractTriggerVolumeComponent.h"

class HealthChargerHITVComponent final : public HoldInteractTriggerVolumeComponent
{
public:
	SET_TYPE_ID(ComponentId::HealthChargerHITV);
	MEMORY_POOL_DECLARE;
	
	const char* GetCompName() override { return "Health Charger HITV Component"; }

	void Initialize() override;
	void DebugUI() override;
	
protected:
	void OnInteract(float deltaTime) override;
	void ResetCharger();

	int mChargeAmount = 50; // charger's remaining charge
	int mMaxChargeAmount = 50; // charger's max charge
	float mDrainRate = 10.0f; // units per second
	float mDrainAccumulator = 0.0f;
};