#pragma once

#include "TypeIds.h"

class SAGE::AnimatorComponent;
class SAGE::TransformComponent;

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

	void CheckForPlayerGameplayInput();
	void UpdateWeaponRotation(const SAGE::Math::Quaternion& rotation);

private:
	void Draw();
	void Idle();
	void Shoot();
	void Reload();
	void Holster();

	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::Graphics::SoundEffectManager* mSoundEffectManager = nullptr;
	SAGE::TransformComponent* mTransformComponent = nullptr;
	SAGE::AnimatorComponent* mAnimatorComponent = nullptr;
	SAGE::Graphics::Animator* mAnimator = nullptr;

	bool mIsEquipped = true;

	// Audio
	SAGE::Graphics::SoundId mShootSoundID = 0;
	float mShootSoundPitchVolume = 0.05f;

};