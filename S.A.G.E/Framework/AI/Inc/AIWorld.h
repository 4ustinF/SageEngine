#pragma once

#include "Entity.h"

namespace AI
{
	class AIWorld
	{
	public:
		//using Obstacles = std::vector<SAGE::Math::Circle>;
		//using Walls = std::vector<SAGE::Math::LineSegment>;

		void RegisterEntity(Entity* entity);
		void UnRegisterEntity(Entity* entity);

		//void AddObstacle(const SAGE::Math::Circle& obstacle);
		//void AddWall(const SAGE::Math::LineSegment& wall);

		Entity* GetClosest(const SAGE::Math::Vector2 position, uint32_t typeID) const;

		const auto& GetAllEntities() const { return mEntities; };

		//const auto& GetObstacles() const { return mObstacles; }
		//const auto& GetWalls() const { return mWalls; }

		uint32_t GetnextID() const
		{
			ASSERT(mNextID < UINT32_MAX, "AIWorld -- Run out of IDS!");
			return mNextID++;
		}

		using EntityFilter = std::function<bool(Entity*)>;


	private:
		EntityPtrs mEntities;

		//Obstacles mObstacles;
		//Walls mWalls;

		mutable uint32_t mNextID = 0;
	};
}