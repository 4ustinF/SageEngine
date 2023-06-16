#include "Precompiled.h"
#include "AlignmentBehavior.h"

#include "Agent.h"

using namespace AI;
using namespace SAGE;
using namespace SAGE::Math;

Vector2 AlignmentBehavior::Calculate(Agent& agent)
{
	if (agent.mNeighbors.empty()) {
		return {};
	}

	Vector2 Alignment = agent.heading;

	for (auto& n : agent.mNeighbors) {
		Alignment += n->heading;
	}

	Alignment /= agent.mNeighbors.size() + 1;
	Alignment -= agent.heading;

	Alignment *= agent.maxSpeed;

	if (IsDebug())
	{
		
	}

	return Alignment;
}