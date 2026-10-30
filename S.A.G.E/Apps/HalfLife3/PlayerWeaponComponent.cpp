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

MEMORY_POOL_DEFINE(PlayerWeaponComponent, 10);

void PlayerWeaponComponent::LoadComponentFromTemplate(const rj::Value& value)
{
	if (value.HasMember("Damage"))
	{
		mDammage = value["Damage"].GetInt();
	}

	if (value.HasMember("Max Ammo"))
	{
		mMaxAmmo = value["Max Ammo"].GetInt();
	}

	if (value.HasMember("Max Ammo Reserve"))
	{
		mMaxAmmoReserve = value["Max Ammo Reserve"].GetInt();
	}
}

void PlayerWeaponComponent::SaveComponentToTemplate(rj::Value& compObj, rj::MemoryPoolAllocator<rj::CrtAllocator>& allocator)
{
	SaveNumberToTemplate(compObj, allocator, "Damage", mDammage);
	SaveNumberToTemplate(compObj, allocator, "Max Ammo", mMaxAmmo);
	SaveNumberToTemplate(compObj, allocator, "Max Ammo Reserve", mMaxAmmoReserve);
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

	mCurrentAmmo = mMaxAmmo;
	mCurrentAmmoReserve = mMaxAmmoReserve;
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
		ImGui::DragFloat("Shoot Sound Pitch Volume##PlayerWeaponComponent", &mShootSoundPitchVolume, 0.01f, 0.0f, 1.0f);
		
		ImGui::DragInt("Current Ammo##PlayerWeaponComponent", &mCurrentAmmo, 1, 0, mMaxAmmo);
		ImGui::DragInt("Max Ammo##PlayerWeaponComponent", &mMaxAmmo, 1, 0, 100);
		
		ImGui::DragInt("Current Ammo Reserve##PlayerWeaponComponent", &mCurrentAmmoReserve, 1, 0, mMaxAmmoReserve);
		ImGui::DragInt("Max Ammo Reserve##PlayerWeaponComponent", &mMaxAmmoReserve, 1, 0, 500);
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

void PlayerWeaponComponent::OnAmmoPickedUp(int ammoAmount)
{
	mCurrentAmmoReserve = Math::Clamp(mCurrentAmmoReserve + Abs(ammoAmount), 0, mMaxAmmoReserve);
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
	if (mCurrentAmmo == 0)
	{
		Reload();
		return;
	}

	mAnimator->PlayAnimation(4, false);
	mSoundEffectManager->PlayOneShotWithPitchOffset(mShootSoundID, mShootSoundPitchVolume);
	mCurrentAmmo -= 1;

	// TODO: Clean up
	GameWorld& world = GetOwner().GetWorld();
	RBPhysicsService* phsyicsService = world.GetService<RBPhysicsService>();
	RBPhysicsWorld& physicsWorld = phsyicsService->GetPhysicsWorld();

	CameraService* mCameraService = world.GetService<CameraService>();
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

void PlayerWeaponComponent::Reload()
{
	if (mCurrentAmmoReserve == 0)
	{
		return;
	}

	mAnimator->PlayAnimation(5, false);

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
}

void PlayerWeaponComponent::Holster()
{
	mIsEquipped = false;
	mAnimator->PlayAnimation(6, false);
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
