#pragma once

#include "TypeIds.h"

class SAGE::RBPhysicsService;
class SAGE::CameraService;
class SAGE::AnimatorComponent;
class SAGE::TransformComponent;
class BulletHolesPoolComponent;
class PlayerControllerComponent;

class PlayerWeaponComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PlayerWeapon);
	MEMORY_POOL_DECLARE;

	enum WeaponState
	{
		Drawing,
		Idling,
		Shooting,
		Reloading,
		Holstering,
		Holstered
	};

	const char* GetCompName() override { return "Player Weapon Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void OnQueueUpdate(float deltaTime) override;
	void DebugUI() override;
	void OnEnable() override;
	void OnDisable() override;

	void CheckForPlayerGameplayInput();
	void UpdateWeaponRotation(const SAGE::Math::Quaternion& rotation);
	void OnAmmoPickedUp(int ammoAmount);

	bool IsBusy() const { return mWeaponState != WeaponState::Idling; }

private:
	friend class PlayerControllerComponent;
	void StartDraw();
	void FinishDraw();
	void Idle();
	void StartShooting();
	void FinishShooting();
	void StartReloading();
	void FinishReloading();
	void StartHolstering();
	void FinishHolstering();

	void OnAnimationFinishedCallback(size_t clipIndex);
	SAGE::Core::Delegate::FDelegateHandle mOnAnimationFinishedHandle;

	BulletHolesPoolComponent* GetOrCreateBulletPool();

	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::Graphics::SoundEffectManager* mSoundEffectManager = nullptr;
	SAGE::RBPhysicsService* mPhysicsService = nullptr;
	SAGE::CameraService* mCameraService = nullptr;
	SAGE::TransformComponent* mTransformComponent = nullptr;
	SAGE::AnimatorComponent* mAnimatorComponent = nullptr;
	SAGE::Graphics::Animator* mAnimator = nullptr;
	BulletHolesPoolComponent* mBulletPool = nullptr;

	WeaponState mWeaponState = WeaponState::Holstered;
	int mDamage = 0;
	int mCurrentAmmo = 0;
	int mMaxAmmo = 0;
	int mCurrentAmmoReserve = 0;
	int mMaxAmmoReserve = 0;
	bool mIsSemiAutomatic = false;

	// Fire rate
	void ResetCanShootTimer();
	bool mCanShoot = true;
	int mRateOfFire = 60; // In RPM
	float mCanShootTimer = 1.0f;
	float mRateOfFireInSeconds = 1.0f;

	// Animations
	const size_t mIdlingAnimIndex = 0; // TODO: maybe turn this into a non looping anim and swap between idle anims?
	const size_t mDrawAnimIndex = 1;
	const size_t mReloadAnimIndex = 2;
	size_t mHolsteringAnimIndex = -1; // 3
	std::vector<size_t> mShootingAnimIndexs;

	// Audio
	std::vector<std::string> mShootSoundFilePaths;
	std::vector<SAGE::Graphics::SoundId> mShootSoundIDs;
	float mShootSoundPitchVolume = 0.05f;

};