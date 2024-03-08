#pragma once

#include "Entity.h"

namespace SAGE::AI
{
	class Agent;

	using AgentPtrs = std::vector<Agent*>;

	class Agent : public Entity
	{
	public:
		Agent(AIWorld& world, uint32_t typeID);
		~Agent() override = default;

		const Agent* target = nullptr;
		std::vector<Agent*> mNeighbors;
		SAGE::Math::Vector2 velocity = SAGE::Math::Vector2::Zero;
		SAGE::Math::Vector2 destination = SAGE::Math::Vector2::Zero;
		float maxSpeed = 100.0f;
		float mass = 1.0f;
	};
}