#include "MobService.h"
#include "Enums.h"

#include "AnimalComponent.h"

using namespace SAGE;
using namespace SAGE::Math;

void MobService::Initialize()
{
	SetServiceName("Mob Service");

	mPassiveMobHandles.reserve(mMaxPassiveMobs);
}

void MobService::Terminate()
{
	mPassiveMobHandles.clear();
}

void MobService::Update(float deltaTime)
{
	for (int i = 0; i < mPassiveMobHandles.size(); ++i)
	{
		auto mob = GetWorld().GetGameObject(mPassiveMobHandles[i]);

		std::vector<GameObjectHandle>::iterator it = mPassiveMobHandles.begin();
		while (it != mPassiveMobHandles.end()) 
		{
			if (mob == nullptr || mob->GetComponent<TransformComponent>()->position.y < 0.0f)
			{
				if (mob != nullptr)
				{
					GetWorld().DestroyGameObject(mPassiveMobHandles[i]);
				}
				it = mPassiveMobHandles.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void MobService::Render()
{

}

void MobService::DebugUI()
{
	if (ImGui::Button("Create Pig"))
	{
		SpawnAnimals(Vector3(5.0f, 25.0f, 5.0f));
	}

	if (ImGui::Button("Kill Pigs"))
	{
		for (auto mobHandle : mPassiveMobHandles)
		{
			GetWorld().DestroyGameObject(mobHandle);
		}
		mPassiveMobHandles.clear();
	}
}

void MobService::RegisterPlayer(SAGE::GameObjectHandle handle)
{
	mPlayerHandle = handle;
}

void MobService::Register(SAGE::GameObjectHandle handle)
{
	mPassiveMobHandles.push_back(handle);
}

void MobService::Unregister(const SAGE::GameObjectHandle handle)
{
	for (int i = 0; i < mPassiveMobHandles.size(); ++i)
	{
		auto obj = GetWorld().GetGameObject(mPassiveMobHandles[i]);
		if (obj == GetWorld().GetGameObject(handle))
		{
			std::swap(mPassiveMobHandles[i], mPassiveMobHandles.back());
			GetWorld().DestroyGameObject(handle);
			mPassiveMobHandles.pop_back();
			break;
		}
	}
}

void MobService::SpawnAnimals(const Vector3& position)
{
	//if (Random::UniformFloat() > 0.1f)
	//{
	//	return;
	//}

	const int spawnAmount = 1; // Random::UniformInt(1, 4);

	for (int i = 0; i < spawnAmount; ++i)
	{
		// AnimalType::Pig
		auto handle = GetWorld().CreateGameObject("../../Assets/Templates/VoxelPig.json")->GetHandle();
		auto animal = GetWorld().GetGameObject(handle);
		const std::string name = "Pig_" + std::to_string(mPassiveMobHandles.size());
		animal->SetName(name.c_str());
		animal->GetComponent<TransformComponent>()->SetPosition(position);
		//animal->SetActive(false);
	}
}

void MobService::AttackMob(int index, int dmg, const Vector3& dir)
{
	for (auto& mobHandle : mPassiveMobHandles)
	{
		auto mob = GetWorld().GetGameObject(mobHandle);
		if (!mob->IsActive())
		{
			continue;
		}

		auto mobRB = mob->GetComponent<RigidBodyComponent>()->GetRigidBody();
		if (mobRB->getUserIndex() == index)
		{
			mob->GetComponent<AnimalComponent>()->TakeDamage(dmg, dir);
		}
	}
}