#include "AnimalComponent.h"

#include "SAGE/Inc/GameObject.h"
#include "SAGE/Inc/TransformComponent.h"
#include "MobService.h"
#include "PlayerControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Input;

MEMORY_POOL_DEFINE(AnimalComponent, 100);

void AnimalComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	mMobService = GetOwner().GetWorld().GetService<MobService>();
	mMobService->Register(GetOwner().GetHandle());
}

void AnimalComponent::Terminate()
{
	mMobService = nullptr;
	mAnimalRB = nullptr;
}

void AnimalComponent::Update(float deltaTime)
{
	auto inputSystem = InputSystem::Get();
	float amount = 5.0f;

	if (inputSystem->IsKeyDown(KeyCode::RIGHT))
	{
		mAnimalRB->applyTorque(btVector3(0, amount, 0));
	}

	if (inputSystem->IsKeyDown(KeyCode::LEFT))
	{
		mAnimalRB->applyTorque(btVector3(0, -amount, 0));
	}

	//Wander(deltaTime);
}

void AnimalComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Animal Component##AnimalComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::InputFloat("Wander Radius Speed##AnimalComponent", &mWanderRadius, 0.01f, 0.05f);
		ImGui::InputFloat("Wander Distance Speed##AnimalComponent", &mWanderDistance, 0.01f, 0.05f);
		ImGui::InputFloat("Wander Jitter Speed##AnimalComponent", &mWanderJitter, 0.01f, 0.05f);
		ImGui::InputFloat("Walk Speed##AnimalComponent", &mWalkSpeed, 0.01f, 0.05f);
	}

	SimpleDraw::AddSphere(mProjectedPosition, 15, 15, mWanderRadius, Colors::Red);
	SimpleDraw::AddSphere(mWorldWanderTarget, 5, 5, 0.05f, Colors::Cyan);
	SimpleDraw::AddLine(mTransformComponent->position, mWorldWanderTarget, Colors::Cyan);

	//Vector3 dir = Normalize(mWorldWanderTarget - mTransformComponent->position);
	//SimpleDraw::AddLine(mTransformComponent->position, mTransformComponent->position + dir * mWanderDistance, Colors::Cyan);
}

void AnimalComponent::OnEnable()
{
	mAnimalRB = GetOwner().GetComponent<RigidBodyComponent>()->GetRigidBody();
	mAnimalRB->setActivationState(DISABLE_DEACTIVATION);
}

void AnimalComponent::OnDisable()
{

}

void AnimalComponent::Wander(float deltaTime)
{
	// Applied random jitter to wander target
	auto newWanderTarget = mLocalWanderTarget + Random::OnUnitCircle() * mWanderJitter;
	newWanderTarget = Normalize(newWanderTarget) * mWanderRadius;
	mLocalWanderTarget = newWanderTarget;

	// Project the new position infront of the agent
	mProjectedPosition = mTransformComponent->position + GetLook(mTransformComponent->GetMatrix4()) * mWanderDistance;
	newWanderTarget += Vector2(mProjectedPosition.x, mProjectedPosition.z);

	// Transform target to world space
	Matrix4 m = mTransformComponent->GetMatrix4();
	const Matrix3 worldTransform = Matrix3(m._11, m._12, m._13, m._21, m._22, m._23, m._31, m._32, m._33);
	const Vector2 wanderTarget = TransformCoord(newWanderTarget, worldTransform);
	mWorldWanderTarget = Vector3(wanderTarget.x, mProjectedPosition.y, wanderTarget.y);

	Vector3 dir = Normalize(mWorldWanderTarget - mTransformComponent->position);
	dir.y = 0.0f;
	mAnimalRB->applyCentralImpulse(ConvertToBtvector3(dir * mWalkSpeed * deltaTime));
}

void AnimalComponent::TakeDamage(int damage, const Vector3& dir)
{
	mCurrentHealth -= damage;
	mAnimalRB->applyCentralImpulse(ConvertToBtvector3(dir * mKnockBackAmount));

	const auto& playerParticleComponent = GetOwner().GetComponent<ParticleComponent>();
	const Vector3 pos = GetOwner().GetComponent<TransformComponent>()->position;

	for (int i = 0; i < mBloodParticleCount; ++i)
	{
		ParticleData particle;
		particle.SetPosition(pos);
		particle.SetVelocity(Random::OnUnitSphere() * 0.02f);
		particle.lifeLength = 0.5f;
		particle.gravityEffect = 0.3f;

		particle.currentColor = Colors::Red;

		particle.scaleOverTime = true;
		particle.startScale = 0.3f;
		particle.endScale = 0.0f;
		particle.currentScale = particle.startScale;

		particle.rotateOverTime = true;
		particle.endRotation = Random::UniformFloat(-180.0f, 180.0f);

		playerParticleComponent->AddParticle(particle);
	}

	if (mCurrentHealth <= 0) // Die
	{
		GiveDeathDrops();
		mMobService->Unregister(GetOwner().GetHandle());
	}
}

#pragma region ---Drops---

void AnimalComponent::GiveDeathDrops()
{
	switch (mAnimalType)
	{
	case AnimalType::Chicken:
		GiveChickenDrops();
		break;
	case AnimalType::Cow:
		GiveCowDrops();
		break;
	case AnimalType::Pig:
		GivePigDrops();
		break;
	case AnimalType::Sheep:
		GiveSheepDrops();
		break;
	}
}

void AnimalComponent::GiveChickenDrops()
{
	auto player = GetOwner().GetWorld().GetGameObject(mMobService->GetPlayerHandle());

	// XP
	//const int xpAmount = Random::UniformInt(1, 3);

	// Food
	const int foodAmount = Random::UniformInt(1, 3);
	player->GetComponent<PlayerControllerComponent>()->AddItemToInventory(ItemTypes::Bread); //TODO: Add Chicken

	// Extra Drops
	//const int featherAmount = Random::UniformInt(0, 2);
	//for (int i = 0; i < featherAmount; ++i)
	//{
	//	player->GetComponent<PlayerControllerComponent>()->AddItemToInventory(ItemTypes::Feather); //TODO: Add Feathers
	//}
}

void AnimalComponent::GiveCowDrops()
{
	auto player = GetOwner().GetWorld().GetGameObject(mMobService->GetPlayerHandle());

	// XP
	//const int xpAmount = Random::UniformInt(1, 3);

	// Food
	const int foodAmount = Random::UniformInt(1, 3);
	for (int i = 0; i < foodAmount; ++i)
	{
		player->GetComponent<PlayerControllerComponent>()->AddItemToInventory(ItemTypes::Bread); //TODO: Add steak
	}

	// Extra Drops
	//const int leatherAmount = Random::UniformInt(0, 2);
	//for (int i = 0; i < leatherAmount; ++i)
	//{
	//	player->GetComponent<PlayerControllerComponent>()->AddItemToInventory(ItemTypes::Leather); //TODO: Add leather
	//}
}

void AnimalComponent::GivePigDrops()
{
	auto player = GetOwner().GetWorld().GetGameObject(mMobService->GetPlayerHandle());

	// XP
	//const int xpAmount = Random::UniformInt(1, 3);

	// Food
	const int foodAmount = Random::UniformInt(1, 3);
	for (int i = 0; i < foodAmount; ++i)
	{
		player->GetComponent<PlayerControllerComponent>()->AddItemToInventory(ItemTypes::Bread); //TODO: Add porkchops
	}
}

void AnimalComponent::GiveSheepDrops()
{
	auto player = GetOwner().GetWorld().GetGameObject(mMobService->GetPlayerHandle());

	// XP
	//const int xpAmount = Random::UniformInt(1, 3);

	// Food
	const int foodAmount = Random::UniformInt(1, 2);
	for (int i = 0; i < foodAmount; ++i)
	{
		player->GetComponent<PlayerControllerComponent>()->AddItemToInventory(ItemTypes::Bread); //TODO: Add lamb
	}

	// Extra Drops
	//player->GetComponent<PlayerControllerComponent>()->AddItemToInventory(ItemTypes::Wool); //TODO: Wool

}

#pragma endregion