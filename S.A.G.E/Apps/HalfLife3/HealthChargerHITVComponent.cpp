#include "SAGE/Inc/Precompiled.h"
#include "HealthChargerHITVComponent.h"

#include "SAGE/Inc/MeshRendererComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;
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

	mSoundEffectManager = SoundEffectManager::Get();
	mMedShot4SoundID = mSoundEffectManager->Load("items/medshot4.wav");
	mMedCharge4SoundID = mSoundEffectManager->Load("items/medcharge4.wav");
	mMedShotNo1SoundID = mSoundEffectManager->Load("items/medshotno1.wav");


}

void HealthChargerHITVComponent::Terminate()
{
	mMedShot4SoundID = 0;
	mMedCharge4SoundID = 0;
	mMedShotNo1SoundID = 0;
	mSoundEffectManager = nullptr;
	mFrontMedkitMeshRenderer = nullptr;
	HoldInteractTriggerVolumeComponent::Terminate();
}

void HealthChargerHITVComponent::OnEnable()
{
	HoldInteractTriggerVolumeComponent::OnEnable();

	if (mFrontMedkitMeshRenderer == nullptr)
	{
		if (GameObject* frontMedkit = GetOwner().FindChildByName("Medkit Front"))
		{
			mFrontMedkitMeshRenderer = frontMedkit->GetComponent<MeshRendererComponent>();
		}
	}
}

void HealthChargerHITVComponent::OnInteractStart()
{
	mSoundEffectManager->Play(mMedShot4SoundID, false, mAudioVolume);
	mSoundEffectManager->Play(mMedCharge4SoundID, true, mAudioVolume);
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
		OnEmptiedCharger();
	}
}

void HealthChargerHITVComponent::OnInteractEnd()
{
	mSoundEffectManager->Stop(mMedCharge4SoundID);
}

void HealthChargerHITVComponent::OnEmptiedCharger()
{
	SetCanTrigger(false);
	mSoundEffectManager->Play(mMedShotNo1SoundID, false, mAudioVolume);

	if (MeshRendererComponent* frontMedkitMeshRenderer = GetFrontMedkitMeshRenderer())
	{
		frontMedkitMeshRenderer->SetDiffuseMapFileName("D:/GitHubFiles/SageEngine/S.A.G.E/Assets/Models/HalfLife/Misc/Materials/HealthCharger/_1MEDKIT.png");
	}
}

void HealthChargerHITVComponent::ResetCharger()
{
	if (MeshRendererComponent* frontMedkitMeshRenderer = GetFrontMedkitMeshRenderer())
	{
		frontMedkitMeshRenderer->SetDiffuseMapFileName("D:/GitHubFiles/SageEngine/S.A.G.E/Assets/Models/HalfLife/Misc/Materials/HealthCharger/_0MEDKIT.png");
	}

	mDrainAccumulator = 0.0f;
	mChargeAmount = mMaxChargeAmount;
	SetCanTrigger(true);
}

MeshRendererComponent* HealthChargerHITVComponent::GetFrontMedkitMeshRenderer()
{
	if (mFrontMedkitMeshRenderer == nullptr)
	{
		if (GameObject* frontMedkit = GetOwner().FindChildByName("Medkit Front"))
		{
			mFrontMedkitMeshRenderer = frontMedkit->GetComponent<MeshRendererComponent>();
		}
	}

	return mFrontMedkitMeshRenderer;
}