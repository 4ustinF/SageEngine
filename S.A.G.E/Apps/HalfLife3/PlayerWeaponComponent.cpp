#include "SAGE/Inc/Precompiled.h"
#include "PlayerWeaponComponent.h"

#include "SAGE/Inc/AnimatorComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;
namespace rj = rapidjson;

MEMORY_POOL_DEFINE(PlayerWeaponComponent, 10);

void PlayerWeaponComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	
}

void PlayerWeaponComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{

}

void PlayerWeaponComponent::Initialize()
{
	mInputSystem = InputSystem::Get();

	GameObject& owner = GetOwner();
	mTransformComponent = owner.GetComponent<TransformComponent>();
	mAnimatorComponent = owner.GetComponent<AnimatorComponent>();
	mAnimator = &mAnimatorComponent->GetAnimator();

	mSoundEffectManager = SoundEffectManager::Get();
	mShootSoundID = mSoundEffectManager->Load("weapons/pl_gun3.wav");
}

void PlayerWeaponComponent::Terminate()
{
	mShootSoundID = 0;

	mAnimator = nullptr;
	mAnimatorComponent = nullptr;
	mTransformComponent = nullptr;
	mSoundEffectManager = nullptr;
	mInputSystem = nullptr;
}

void PlayerWeaponComponent::Update(float deltaTime)
{

}

void PlayerWeaponComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Weapon Component##PlayerWeaponComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Shoot Sound Pitch Volume", &mShootSoundPitchVolume, 0.01f, 0.0f, 1.0f);
	}
}

void PlayerWeaponComponent::CheckForPlayerGameplayInput()
{
	if (!mIsEquipped)
	{
		if (mInputSystem->IsKeyPressed(KeyCode::ONE))
		{
			Draw();
		}
	}
	else
	{
		if (mAnimator->IsFinished())
		{
			Idle();
		}

		if (mAnimator->GetIsLooping())
		{
			if (mInputSystem->IsKeyPressed(KeyCode::ZERO))
			{
				Holster();
			}
			else if (mInputSystem->IsMousePressed(MouseButton::LBUTTON))
			{
				Shoot();
			}
			else if (mInputSystem->IsKeyPressed(KeyCode::R))
			{
				Reload();
			}
		}
	}
}

void PlayerWeaponComponent::UpdateWeaponRotation(const Quaternion& rotation)
{
	mTransformComponent->SetLocalRotation(rotation);
}

void PlayerWeaponComponent::Draw()
{
	mIsEquipped = true;
	mAnimator->PlayAnimation(3, false);
}

void PlayerWeaponComponent::Idle()
{
	mAnimator->PlayAnimation(0, true);
}

void PlayerWeaponComponent::Shoot()
{
	mAnimator->PlayAnimation(4, false);
	mSoundEffectManager->PlayOneShotWithPitchOffset(mShootSoundID, mShootSoundPitchVolume);
}

void PlayerWeaponComponent::Reload()
{
	mAnimator->PlayAnimation(5, false);
}

void PlayerWeaponComponent::Holster()
{
	mIsEquipped = false;
	mAnimator->PlayAnimation(6, false);
}
