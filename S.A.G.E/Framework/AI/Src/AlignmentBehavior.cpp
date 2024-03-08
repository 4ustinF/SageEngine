#include "Precompiled.h"
#include "AlignmentBehavior.h"

#include "Agent.h"

using namespace SAGE::AI;
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

	Alignment /= static_cast<float>(agent.mNeighbors.size() + 1);
	Alignment -= agent.heading;

	Alignment *= agent.maxSpeed;

	if (IsDebug())
	{
		
	}

	return Alignment;
}