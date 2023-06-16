#pragma once

#include "TypeIds.h"

class MobService final : public SAGE::Service
{
public:
	SET_TYPE_ID(ServiceId::Mob)

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;

	void Render() override;
	void DebugUI() override;
	
	void RegisterPlayer(SAGE::GameObjectHandle handle);
	SAGE::GameObjectHandle GetPlayerHandle() { return mPlayerHandle; }

	void Register(SAGE::GameObjectHandle handle);
	void Unregister(const SAGE::GameObjectHandle handle);

	void SpawnAnimals(const SAGE::Math::Vector3& position);
	void AttackMob(int index, int dmg, const SAGE::Math::Vector3& dir);

private:
	int mMaxPassiveMobs = 10;
	std::vector<SAGE::GameObjectHandle> mPassiveMobHandles;
	SAGE::GameObjectHandle mPlayerHandle;
};