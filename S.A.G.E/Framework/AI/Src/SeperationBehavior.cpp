#include "Precompiled.h"
#include "SeperationBehavior.h"

#include "Agent.h"

using namespace AI;
using namespace SAGE;
using namespace SAGE::Math;

Vector2 SeperationBehavior::Calculate(Agent& agent)
{
	if (agent.mNeighbors.empty()) {
		return {};
	}

	Vector2 seperation;

	for (auto& n : agent.mNeighbors)
	{
		Vector2 vec = agent.position - n->position;

		vec = Normalize(vec);

		float distance = sqrt((agent.position.x - n->position.x) * (agent.position.x - n->position.x)
			+ (agent.position.y - n->position.y) * (agent.position.y - n->position.y));

		vec /= distance;

		vec *= agent.maxSpeed;

		seperation += vec;
	}

	if (IsDebug())
	{

	}
	
	return seperation;
}