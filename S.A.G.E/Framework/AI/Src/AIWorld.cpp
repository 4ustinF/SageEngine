#include "Precompiled.h"
#include "AIWorld.h"

using namespace SAGE::AI;
using namespace SAGE;
using namespace SAGE::Math;

void AIWorld::RegisterEntity(Entity* entity)
{
	ASSERT(std::find(mEntities.begin(), mEntities.end(), entity) == mEntities.end(), "AIWorld -- Entity already registered!");
	mEntities.push_back(entity);
}

void AIWorld::UnRegisterEntity(Entity* entity)
{
	auto iter = std::find(mEntities.begin(), mEntities.end(), entity);
	if (iter != mEntities.end())
	{
		std::iter_swap(iter, mEntities.end() - 1); //Constant time removal
		mEntities.pop_back();
	}
}

//void AIWorld::AddObstacle(const Circle& obstacle)
//{
//
//}
//
//void AIWorld::AddWall(const LineSegment& wall)
//{
//
//}

Entity* AIWorld::GetClosest(const Vector2 position, uint32_t typeID) const
{
	Entity* closestEntity = nullptr;
	float closestDistanceSqr = FLT_MAX;

	for (auto entity : mEntities) {
		if (entity->GetTypeID() == typeID ) {
			const float distSqr = DistanceSqr(position, entity->position);

			if (distSqr < closestDistanceSqr) {
				closestEntity = entity;
				closestDistanceSqr = distSqr;
			}
		}
	}
	return closestEntity;
}
