#include "Precompiled.h"
#include "SpawnerComponent.h"
#include "TransformComponent.h"

#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;

MEMORY_POOL_DEFINE(SpawnerComponent, 1000);

void SpawnerComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
}

void SpawnerComponent::Update(float deltaTime)
{
	if (mSpawnCount >= mMaxSpawnCount) { return; }

	mTimer += deltaTime;
	if (mTimer < mWaitTime) { return; }
	mTimer = 0.0f;
	++mSpawnCount;

	auto gameObject = GetOwner().GetWorld().CreateGameObject(mFileName);
	auto transformComponent = gameObject->GetComponent<TransformComponent>();
	if (transformComponent)
	{
		transformComponent->position = mTransformComponent->position;
		transformComponent->rotation = mTransformComponent->rotation;
		transformComponent->scale = mTransformComponent->scale;
	}
}
