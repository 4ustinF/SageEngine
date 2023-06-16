#pragma once

namespace AI
{
	class AIWorld;
	class Entity;

	using EntityPtrs = std::vector<Entity*>;

	class Entity
	{
	public:
		Entity(AIWorld& world, uint32_t typeID);
		virtual ~Entity();

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(Entity&&) = delete;

		AIWorld& world;
		SAGE::Math::Vector2 position = SAGE::Math::Vector2::Zero;
		SAGE::Math::Vector2 heading = SAGE::Math::Vector2::YAxis;
		float radius = 1.0f;

		SAGE::Math::Matrix3 GetWorldTransform() const;

		uint32_t GetTypeID() const { return static_cast<uint32_t>(mUniqueID >> 32); }
		uint64_t GetUniqueID() const { return mUniqueID; }


	private:
		const uint64_t mUniqueID = 0;

	};
}