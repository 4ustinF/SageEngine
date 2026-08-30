#include "SAGE/Inc/Precompiled.h"
#include "HealthChargerHITVComponent.h"

using namespace SAGE;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(HealthChargerHITVComponent, 50);

void HealthChargerHITVComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Heatlh Charger HITV Component##HealthChargerHITVComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::SliderInt("Charges Remaining##HealthChargerHITVComponent", &mChargeAmount, 0, mMaxChargeAmount);
		if (ImGui::Button("Reset Charger##HealthChargerHITVComponent")) { ResetCharger(); }
	}
}

void HealthChargerHITVComponent::Initialize()
{
	HoldInteractTriggerVolumeComponent::Initialize();
	ResetCharger();
}

void HealthChargerHITVComponent::OnInteract(float deltaTime)
{
	mDrainAccumulator += mDrainRate * deltaTime;
	while (mDrainAccumulator >= 1.0f && mChargeAmount > 0) // && playerHealth < maxHealth
	{
		mChargeAmount -= 1;
		//playerHealth += 1;
		mDrainAccumulator -= 1.0f;
	}

	if (mChargeAmount <= 0)
	{
		mCanTrigger = false;
	}
}

void HealthChargerHITVComponent::ResetCharger()
{
	mDrainAccumulator = 0.0f;
	mChargeAmount = mMaxChargeAmount;
	mCanTrigger = true;
}