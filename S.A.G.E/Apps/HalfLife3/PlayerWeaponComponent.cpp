#include "SAGE/Inc/Precompiled.h"
#include "PlayerWeaponComponent.h"

#include "SAGE/Inc/CameraService.h"
#include "SAGE/Inc/RBPhysicsService.h"

#include "SAGE/Inc/AnimatorComponent.h"
#include "BulletHolesPoolComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(PlayerWeaponComponent, 16);

void PlayerWeaponComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("Damage"))
	{
		mDamage = value["Damage"].GetInt();
	}

	if (value.HasMember("Max Ammo"))
	{
		mMaxAmmo = value["Max Ammo"].GetInt();
	}

	if (value.HasMember("Max Ammo Reserve"))
	{
		mMaxAmmoReserve = value["Max Ammo Reserve"].GetInt();
	}

	if (value.HasMember("Fire Rate"))
	{
		mRateOfFire = value["Fire Rate"].GetInt();
		mRateOfFireInSeconds = 60.0f / static_cast<float>(mRateOfFire);
	}

	if (value.HasMember("Is Semi Automatic"))
	{
		mIsSemiAutomatic = value["Is Semi Automatic"].GetBool();
	}

	if (value.HasMember("Holstering Animset Index"))
	{
		mHolsteringAnimIndex = value["Holstering Animset Index"].GetInt();
	}

	if (value.HasMember("Shooting Animsets"))
	{
		const auto& shootingAnimsets = value["Shooting Animsets"].GetArray();
		mShootingAnimIndexs.reserve(shootingAnimsets.Size());
		for (const auto& shootingAnimset : shootingAnimsets)
		{
			mShootingAnimIndexs.push_back(static_cast<size_t>(shootingAnimset.GetInt()));
		}
	}

	if (value.HasMember("Audio Events"))
	{
		const auto& audioEvents = value["Audio Events"].GetArray();
		for (const auto& audioEvent : audioEvents)
		{
			PlayerWeaponAudioEvent playerWeaponAudioEvent;
			playerWeaponAudioEvent.tick = audioEvent["Tick"].GetFloat();
			playerWeaponAudioEvent.eventName = audioEvent["Event Name"].GetString();
			playerWeaponAudioEvent.soundFilePath = audioEvent["Sound File Path"].GetString();
			mAudioEventsMap[audioEvent["Clip Name"].GetString()].push_back(std::move(playerWeaponAudioEvent));
		}
	}

	if (value.HasMember("Shoot Sound IDs"))
	{
		const auto& soundIDs = value["Shoot Sound IDs"].GetArray();
		for (const auto& soundId : soundIDs)
		{
			mShootSoundFilePaths.push_back(soundId.GetString());
		}
	}
}

void PlayerWeaponComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	SaveNumberToTemplate(compObj, allocator, "Damage", mDamage);
	SaveNumberToTemplate(compObj, allocator, "Max Ammo", mMaxAmmo);
	SaveNumberToTemplate(compObj, allocator, "Max Ammo Reserve", mMaxAmmoReserve);
	SaveNumberToTemplate(compObj, allocator, "Fire Rate", mRateOfFire);
	if (mIsSemiAutomatic) { SaveBoolToTemplate(compObj, allocator, "Is Semi Automatic", mIsSemiAutomatic); }
	SaveNumberToTemplate(compObj, allocator, "Holstering Animset Index", mHolsteringAnimIndex, static_cast<size_t>(-1));
	SaveNumbersToTemplate(compObj, allocator, "Shooting Animsets", mShootingAnimIndexs);
	// TODO: Save Audio Events
	SaveStringsToTemplate(compObj, allocator, "Shoot Sound IDs", mShootSoundFilePaths);
}

void PlayerWeaponComponent::Initialize()
{
	mInputSystem = InputSystem::Get();

	GameObject& owner = GetOwner();
	GameWorld& world = owner.GetWorld();
	mPhysicsService = world.GetService<RBPhysicsService>();
	mCameraService = world.GetService<CameraService>();
	mTransformComponent = owner.GetComponent<TransformComponent>();
	mAnimatorComponent = owner.GetComponent<AnimatorComponent>();
	mAnimationBlender = &mAnimatorComponent->GetBlender();

	mSoundEffectManager = SoundEffectManager::Get();
	for (const std::string& soundFilePath : mShootSoundFilePaths)
	{
		mShootSoundIDs.push_back(mSoundEffectManager->Load(soundFilePath.c_str()));
	}

	FOnAnimationEvent& onAnimationEvent = mAnimationBlender->GetOnAnimationEventDelegate();
	mOnAnimationEventHandle = onAnimationEvent.AddRaw(this, &PlayerWeaponComponent::OnAnimationEventCallback);

	for (auto& pair : mAudioEventsMap)
	{
		for (PlayerWeaponAudioEvent& audioEvent : pair.second)
		{
			audioEvent.soundID = mSoundEffectManager->Load(audioEvent.soundFilePath.c_str());
			mAnimatorComponent->AddEvent(pair.first, audioEvent.tick, audioEvent.eventName);
		}
	}

	mCurrentAmmo = mMaxAmmo;
	mCurrentAmmoReserve = mMaxAmmoReserve;
}

void PlayerWeaponComponent::Terminate()
{
	mShootingAnimIndexs.clear();

	mShootSoundFilePaths.clear();
	mShootSoundIDs.clear();
	mAudioEventsMap.clear();

	if (mAnimationBlender != nullptr)
	{
		mAnimationBlender->GetOnAnimationEventDelegate().Remove(mOnAnimationEventHandle);
		mAnimationBlender = nullptr;
	}

	mAnimatorComponent = nullptr;
	mTransformComponent = nullptr;
	
	mPhysicsService = nullptr;
	mCameraService = nullptr;

	mSoundEffectManager = nullptr;
	mInputSystem = nullptr;
}

void PlayerWeaponComponent::Update(float deltaTime)
{

}

void PlayerWeaponComponent::OnQueueUpdate(float deltaTime)
{
	if (!mCanShoot)
	{
		mCanShootTimer -= deltaTime;
		if (mCanShootTimer <= 0.0f)
		{
			ResetCanShootTimer();
		}
		else
		{
			EnqueueUpdate();
		}
	}
}

void PlayerWeaponComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Weapon Component##PlayerWeaponComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Shoot Sound Pitch Volume##PlayerWeaponComponent", &mShootSoundPitchVolume, 0.01f, 0.0f, 1.0f);
		
		ImGui::DragInt("Current Ammo##PlayerWeaponComponent", &mCurrentAmmo, 1, 0, mMaxAmmo);
		ImGui::DragInt("Max Ammo##PlayerWeaponComponent", &mMaxAmmo, 1, 0, 100);
		
		ImGui::DragInt("Current Ammo Reserve##PlayerWeaponComponent", &mCurrentAmmoReserve, 1, 0, mMaxAmmoReserve);
		ImGui::DragInt("Max Ammo Reserve##PlayerWeaponComponent", &mMaxAmmoReserve, 1, 0, 500);

		ImGui::Checkbox("Is Semi-Automatic##PlayerWeaponComponent", &mIsSemiAutomatic);

		ImGui::Checkbox("Can Shoot##PlayerWeaponComponent", &mCanShoot);

		if (ImGui::DragFloat("Rate of Fire (seconds)##PlayerWeaponComponent", &mRateOfFireInSeconds, 0.01f, 0.0f, 3.0f))
		{
			ResetCanShootTimer();
		}
	}
}

void PlayerWeaponComponent::OnEnable()
{
	if (mAnimatorComponent)
	{
		mOnAnimationFinishedHandle = mAnimatorComponent->GetOnAnimationFinishedDelegate().AddRaw(this, &PlayerWeaponComponent::OnAnimationFinishedCallback);
	}

	ResetCanShootTimer();
	StartDraw();
}

void PlayerWeaponComponent::OnDisable()
{
	if (mAnimatorComponent)
	{
		mAnimatorComponent->GetOnAnimationFinishedDelegate().Remove(mOnAnimationFinishedHandle);
	}
}

void PlayerWeaponComponent::CheckForPlayerGameplayInput()
{
	if (mWeaponState == WeaponState::Holstered) // TODO: Check if this is even needed?
	{
		//if (mInputSystem->IsKeyPressed(KeyCode::ONE))
		//{
		//	StartDraw(); // TODO: Won't work anymore. Move out
		//}
		return;
	}

	if (IsBusy())
	{
		if (mWeaponState == WeaponState::Shooting) // Shooting can interrupt shooting now.
		{
			if (mIsSemiAutomatic ? 
				mInputSystem->IsMouseDown(MouseButton::LBUTTON) : mInputSystem->IsMousePressed(MouseButton::LBUTTON))
			{
				StartShooting();
			}
		}

		return;
	}

	if (mInputSystem->IsKeyPressed(KeyCode::ZERO))
	{
		StartHolstering();
	}
	else if (mIsSemiAutomatic ? mInputSystem->IsMouseDown(MouseButton::LBUTTON) : mInputSystem->IsMousePressed(MouseButton::LBUTTON))
	{
		StartShooting();
	}
	else if (mInputSystem->IsKeyPressed(KeyCode::R))
	{
		StartReloading();
	}
}

void PlayerWeaponComponent::UpdateWeaponRotationAndPosition(const Camera& camera)
{
	mTransformComponent->SetLocalRotation(camera.GetOrientation());
	mTransformComponent->SetPosition(camera.GetPosition());
}

void PlayerWeaponComponent::OnAmmoPickedUp(int ammoAmount)
{
	mCurrentAmmoReserve = Math::Clamp(mCurrentAmmoReserve + Abs(ammoAmount), 0, mMaxAmmoReserve);
}

void PlayerWeaponComponent::StartDraw()
{
	mWeaponState = WeaponState::Drawing;
	mAnimationBlender->PlayAnimation(mDrawAnimIndex, false);
}

void PlayerWeaponComponent::FinishDraw()
{
	Idle();
}

void PlayerWeaponComponent::Idle()
{
	mWeaponState = WeaponState::Idling;
	mAnimationBlender->SetBlendWeight(mIdlingAnimIndex, 1.0f);
	mAnimationBlender->PlayAnimation(mIdlingAnimIndex, true, true);
	// TODO: We should probably stop this anim now?
}

void PlayerWeaponComponent::StartShooting()
{
	if (!mCanShoot)
	{
		return;
	}

	if (mCurrentAmmo == 0)
	{
		StartReloading();
		return;
	}

	// State
	mWeaponState = WeaponState::Shooting;

	// Start Cooldown
	mCanShoot = false;
	EnqueueUpdate();

	// Anim
	const int shootingAnimIndexsSize = static_cast<int>(mShootingAnimIndexs.size());
	const size_t shootingIndex = shootingAnimIndexsSize == 1 ? 0 : Random::UniformInt(0, shootingAnimIndexsSize - 1);
	mAnimationBlender->PlayAnimation(mShootingAnimIndexs[shootingIndex], false, true);

	for (size_t index : mShootingAnimIndexs) { mAnimationBlender->SetBlendWeight(index, 0.0f); } // TODO: This is a hack to reset shooting anim weights.
	mAnimationBlender->SetBlendWeight(mShootingAnimIndexs[shootingIndex], 1.0f);

	// Audio
	const int shootingSoundIndexsSize = static_cast<int>(mShootSoundIDs.size());
	const int shootSoundIDIndex = shootingSoundIndexsSize == 1 ? 0 : Random::UniformInt(0, shootingSoundIndexsSize - 1);
	mSoundEffectManager->PlayOneShotWithPitchOffset(mShootSoundIDs[shootSoundIDIndex], mShootSoundPitchVolume);
	mCurrentAmmo -= 1;

	// Bullet
	RBPhysicsWorld& physicsWorld = mPhysicsService->GetPhysicsWorld();
	Camera& camera = mCameraService->GetCamera();

	PhysicsRayHit physicsRayHit;
	PhysicsRay physicsRay = PhysicsRay(camera.GetPosition(), camera.GetDirection());
	
	if (physicsWorld.Raycast(physicsRay, physicsRayHit))
	{
		if (BulletHolesPoolComponent* bulletHolesPool = GetOrCreateBulletPool())
		{
			const Quaternion align = Quaternion::RotationFromTo(Vector3::YAxis, physicsRayHit.normal);
			const float angle = Random::UniformFloat(0.0f, Constants::TwoPi);
			const Quaternion spin = Quaternion::RotationAxis(Vector3::YAxis, angle);
			const Quaternion rotation = align * spin;

			bulletHolesPool->SpawnBulletHole(
				physicsRayHit.impactPoint + (physicsRayHit.normal * 0.01f), // Random::UniformFloat(0.009f, 0.011f)
				rotation,
				Random::UniformFloat(0.14f, 0.18f), // Size
				5.0f								// Life Time
			);
		}
	}
}

void PlayerWeaponComponent::FinishShooting()
{
	// TODO: This is a hack to reset shooting anim weights.
	for (size_t index : mShootingAnimIndexs)
	{
		mAnimationBlender->SetBlendWeight(index, 0.0f);
	}

	Idle();
}

void PlayerWeaponComponent::StartReloading()
{
	if (mCurrentAmmo == mMaxAmmo || mCurrentAmmoReserve == 0)
	{
		return;
	}

	mWeaponState = WeaponState::Reloading;
	mAnimationBlender->PlayAnimation(mReloadAnimIndex, false);
	mAnimationBlender->SetBlendWeight(mReloadAnimIndex, 1.0f);
}

void PlayerWeaponComponent::FinishReloading()
{
	const int ammoNeeded = mMaxAmmo - mCurrentAmmo;
	if (mCurrentAmmoReserve >= ammoNeeded)
	{
		mCurrentAmmo = mMaxAmmo;
		mCurrentAmmoReserve -= ammoNeeded;
	}
	else
	{
		mCurrentAmmo += mCurrentAmmoReserve;
		mCurrentAmmoReserve = 0;
	}

	mAnimationBlender->SetBlendWeight(mReloadAnimIndex, 0.0f);
	Idle();
}

void PlayerWeaponComponent::StartHolstering()
{
	mWeaponState = WeaponState::Holstering;

	if (mHolsteringAnimIndex >= 0) 
	{
		mAnimationBlender->PlayAnimation(mHolsteringAnimIndex, false);
		mAnimationBlender->SetBlendWeight(mHolsteringAnimIndex, 1.0f);
	}
	else // Does not have a holstering anim.
	{
		mAnimationBlender->PlayAnimationInReverse(mDrawAnimIndex, false);
		mAnimationBlender->SetBlendWeight(mDrawAnimIndex, 1.0f);
	}
}

void PlayerWeaponComponent::FinishHolstering()
{
	mWeaponState = WeaponState::Holstered;
	mAnimationBlender->SetBlendWeight(mHolsteringAnimIndex, 0.0f);
	mAnimationBlender->SetBlendWeight(mDrawAnimIndex, 0.0f);
	GetOwner().SetActive(false);
}

void PlayerWeaponComponent::OnAnimationFinishedCallback(size_t clipIndex)
{
	switch (mWeaponState)
	{
	case PlayerWeaponComponent::Drawing:
		FinishDraw();
		break;
	case PlayerWeaponComponent::Shooting:
		FinishShooting();
		break;
	case PlayerWeaponComponent::Reloading:
		FinishReloading();
		break;
	case PlayerWeaponComponent::Holstering:
		FinishHolstering();
		break;
	}
}

void PlayerWeaponComponent::OnAnimationEventCallback(size_t clipIndex, const std::string& eventName)
{
	const std::string clipName = mAnimationBlender->GetAnimator(0)->GetClipNameByIndex(clipIndex); // Weird but works. Figure a better way?
	if (const auto it = mAudioEventsMap.find(clipName); it != mAudioEventsMap.end())
	{
		for (const PlayerWeaponAudioEvent& audioEvent : it->second)
		{
			if (audioEvent.eventName == eventName)
			{
				mSoundEffectManager->PlayOneShotWithPitchOffset(audioEvent.soundID, mShootSoundPitchVolume);
				break;
			}
		}
	}
}

BulletHolesPoolComponent* PlayerWeaponComponent::GetOrCreateBulletPool()
{
	if (mBulletPool != nullptr)
	{
		return mBulletPool;
	}

	GameWorld& world = GetOwner().GetWorld();
	if (GameObject* bulletPoolGO = world.FindGameObject("Bullet Hole Pool"))
	{
		mBulletPool = bulletPoolGO->GetComponent<BulletHolesPoolComponent>();
		return mBulletPool;
	}

	GameObject* bulletPoolGO = world.CreateGameObject("../../Assets/Templates/HalfLifeTemplates/Player/Weapons/BulletHoles/bullet_hole_pool.json");
	mBulletPool = bulletPoolGO->GetComponent<BulletHolesPoolComponent>();

	return mBulletPool;
}

void PlayerWeaponComponent::ResetCanShootTimer()
{
	mCanShootTimer = mRateOfFireInSeconds;
	mCanShoot = true;
}

/*

// setup (after model is enabled)
// mShootLayer = animatorComp->AddBlendLayer();
// animatorComp->GetAnimator().PlayAnimation("Idle", true);

// on fire
//mShootLayer->PlayAnimation("Shoot", false, true);

// every frame
// const float target = mShootLayer->IsFinished() ? 0.0f : 1.0f;
// mShootWeight = MoveToward(mShootWeight, target, 15.0f * dt); // ~0.07s ramp
// animatorComp->GetBlender().SetLayerWeight(0, mShootWeight);

*/