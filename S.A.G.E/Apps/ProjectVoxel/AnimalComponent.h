#pragma once

#include "TypeIds.h"
#include "Enums.h"

class SAGE::TransformComponent;
class MobService;

class AnimalComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::Animal);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void DebugUI() override;
	void OnEnable() override;
	void OnDisable() override;

	void SetAnimalType(AnimalType animalType) { mAnimalType = animalType; }

	void TakeDamage(int damage, const SAGE::Math::Vector3& dir);

private:
	btRigidBody* mAnimalRB = nullptr;
	SAGE::TransformComponent* mTransformComponent = nullptr;
	MobService* mMobService = nullptr;

	void Wander(float deltaTime);

	// Drops
	void GiveDeathDrops();
	void GiveChickenDrops();
	void GiveCowDrops();
	void GivePigDrops();
	void GiveSheepDrops();

	AnimalType mAnimalType = AnimalType::Pig;

	SAGE::Math::Vector2 mLocalWanderTarget = SAGE::Math::Vector2::YAxis;
	SAGE::Math::Vector3 mWorldWanderTarget = SAGE::Math::Vector3::Zero;
	SAGE::Math::Vector3 mProjectedPosition = SAGE::Math::Vector3::Zero;
	float mWanderRadius = 3.0f;
	float mWanderDistance = 4.0f;
	float mWanderJitter = 0.1f;
	float mWalkSpeed = 0.0f;

	const float mKnockBackAmount = 2.5f;

	int mCurrentHealth = 10;
	int mMaxHealth = 10;
	int mBloodParticleCount = 40;
};

// Health
// Pig = 10
// Cow = 10
// Sheep = 8
// Chicken = 4

// XP
// Pig = 1-3
// Cow = 1-3
// Sheep = 1-3
// Chicken = 1-3

// Food
// Pig = 1-3
// Cow = 1-3
// Sheep = 1-2
// Chicken = 1

// Extra drops
// Cow - 0-2 leather
// Sheep - 1 wool
// Chicken - 0-2 feathers