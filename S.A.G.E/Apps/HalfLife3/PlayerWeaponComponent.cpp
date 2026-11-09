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

	if (value.HasMember("Is Semi Automatic"))
	{
		mIsSemiAutomatic = value["Is Semi Automatic"].GetBool();
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
	if (mIsSemiAutomatic) { SaveBoolToTemplate(compObj, allocator, "Is Semi Automatic", mIsSemiAutomatic); }
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
	mAnimator = &mAnimatorComponent->GetAnimator();

	mSoundEffectManager = SoundEffectManager::Get();
	for (const std::string& soundFilePath : mShootSoundFilePaths)
	{
		mShootSoundIDs.push_back(mSoundEffectManager->Load(soundFilePath.c_str()));
	}

	mCurrentAmmo = mMaxAmmo;
	mCurrentAmmoReserve = mMaxAmmoReserve;
}

void PlayerWeaponComponent::Terminate()
{
	mShootSoundFilePaths.clear();
	mShootSoundIDs.clear();

	mAnimator = nullptr;
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
	}
}

void PlayerWeaponComponent::OnEnable()
{
	if (mAnimatorComponent)
	{
		mOnAnimationFinishedHandle = mAnimatorComponent->GetOnAnimationFinishedDelegate().AddRaw(this, &PlayerWeaponComponent::OnAnimationFinishedCallback);
	}

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
		return;
	}

	if (mInputSystem->IsKeyPressed(KeyCode::ZERO))
	{
		StartHolstering();
	}
	else if (mIsSemiAutomatic ?
		mInputSystem->IsMouseDown(MouseButton::LBUTTON) : mInputSystem->IsMousePressed(MouseButton::LBUTTON))
	{
		StartShooting();
	}
	else if (mInputSystem->IsKeyPressed(KeyCode::R))
	{
		StartReloading();
	}
}

void PlayerWeaponComponent::UpdateWeaponRotation(const Quaternion& rotation)
{
	mTransformComponent->SetLocalRotation(rotation);
}

void PlayerWeaponComponent::OnAmmoPickedUp(int ammoAmount)
{
	mCurrentAmmoReserve = Math::Clamp(mCurrentAmmoReserve + Abs(ammoAmount), 0, mMaxAmmoReserve);
}

void PlayerWeaponComponent::StartDraw()
{
	mWeaponState = WeaponState::Drawing;
	mAnimator->PlayAnimation(3, false);
}

void PlayerWeaponComponent::FinishDraw()
{
	Idle();
}

void PlayerWeaponComponent::Idle()
{
	mWeaponState = WeaponState::Idling;
	mAnimator->PlayAnimation(0, true);
}

void PlayerWeaponComponent::StartShooting()
{
	if (mCurrentAmmo == 0)
	{
		StartReloading();
		return;
	}

	mWeaponState = WeaponState::Shooting;
	mAnimator->PlayAnimation(4, false);
	const int shootSoundIDIndex = Random::UniformInt(0, static_cast<int>(mShootSoundIDs.size()) - 1);
	mSoundEffectManager->PlayOneShotWithPitchOffset(mShootSoundIDs[shootSoundIDIndex], mShootSoundPitchVolume);
	mCurrentAmmo -= 1;

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
	Idle();
}

void PlayerWeaponComponent::StartReloading()
{
	if (mCurrentAmmo == mMaxAmmo || mCurrentAmmoReserve == 0)
	{
		return;
	}

	mWeaponState = WeaponState::Reloading;
	mAnimator->PlayAnimation(5, false);
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

	Idle();
}

void PlayerWeaponComponent::StartHolstering()
{
	mWeaponState = WeaponState::Holstering;
	//mAnimator->PlayAnimation(6, false);
	mAnimator->PlayAnimationInReverse(3, false);
}

void PlayerWeaponComponent::FinishHolstering()
{
	mWeaponState = WeaponState::Holstered;
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
