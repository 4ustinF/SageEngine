#pragma once

#include "TypeIds.h"

class SAGE::AnimatorComponent;
class SAGE::MeshRendererComponent;
class SAGE::TransformComponent;

struct BulletHole
{
	SAGE::GameObject* gameObject = nullptr;
	SAGE::MeshRendererComponent* meshRendererComp = nullptr;
	SAGE::TransformComponent* transformComp = nullptr;
	float remainingLifeTime = 1.0f;
};

class BulletHolesPoolComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::BulletHolesPool);
	MEMORY_POOL_DECLARE;

	const char* GetCompName() override { return "Bullet Holes Pool Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;

	void OnQueueUpdate(float deltaTime) override;
	void DebugUI() override;

	void SpawnBulletHole(const SAGE::Math::Vector3& position, const SAGE::Math::Quaternion& rotation, float scale, float lifeTime = 1.0f);

private:
	SAGE::GameObject* GetOrCreateBulletPool();
	SAGE::GameObject* GetOrCreateBullethole();

	std::vector<BulletHole> mActiveBulletHoles;
	std::vector<BulletHole> mDisabledBulletHoles;
};