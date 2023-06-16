#pragma once

#include "Component.h"

namespace SAGE
{
	class TransformComponent;

	struct SpawnRule
	{
		virtual bool Check() const = 0;
	};
	class SpawnerComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Spawner)
		MEMORY_POOL_DECLARE

		void Initialize() override;
		void Update(float deltaTime) override;

		void SetFileName(const char* fileName) { mFileName = fileName; }
		void SetMaxSpawnCount(int count) { mMaxSpawnCount = count; }
		void SetWaitTime(float waitTime) { mWaitTime = waitTime; }

	private:
		const TransformComponent* mTransformComponent = nullptr;
		std::filesystem::path mFileName;
		int mSpawnCount = 0;
		int mMaxSpawnCount = 1;
		float mWaitTime = 1.0f;
		float mTimer = 0.0f;
	};
}