#pragma once

#include "TypeIds.h"

class SAGE::AnimatorComponent;
class SAGE::TransformComponent;
class BulletHolesPoolComponent;
class PlayerControllerComponent;

class PlayerWeaponComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PlayerWeapon);
	MEMORY_POOL_DECLARE;

	const char* GetCompName() override { return "Player Weapon Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;
	void OnEnable() override;
	void OnDisable() override;

	void CheckForPlayerGameplayInput();
	void UpdateWeaponRotation(const SAGE::Math::Quaternion& rotation);
	void OnAmmoPickedUp(int ammoAmount);

	bool IsBusy() const { return mIsBusy; };

private:
	friend class PlayerControllerComponent;
	void Draw();
	void Idle();
	void Shoot();
	void Reload();
	void Holster(); // We need callbacks so this deactivates itself after holster animation is done.

	void OnAnimationFinishedCallback(size_t clipIndex);
	SAGE::Core::Delegate::FDelegateHandle mOnAnimationFinishedHandle;

	BulletHolesPoolComponent* GetOrCreateBulletPool();

	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::Graphics::SoundEffectManager* mSoundEffectManager = nullptr;
	SAGE::TransformComponent* mTransformComponent = nullptr;
	SAGE::AnimatorComponent* mAnimatorComponent = nullptr;
	SAGE::Graphics::Animator* mAnimator = nullptr;
	BulletHolesPoolComponent* mBulletPool = nullptr;

	bool mIsEquipped = true;
	int mDamage = 0;
	int mCurrentAmmo = 0;
	int mMaxAmmo = 0;
	int mCurrentAmmoReserve = 0;
	int mMaxAmmoReserve = 0;
	bool mIsBusy = false;

	// Audio
	std::vector<std::string> mShootSoundFilePaths;
	std::vector<SAGE::Graphics::SoundId> mShootSoundIDs;
	float mShootSoundPitchVolume = 0.05f;

};