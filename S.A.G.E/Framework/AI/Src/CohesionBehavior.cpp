#include "Precompiled.h"
#include "CohesionBehavior.h"

#include "Agent.h"

using namespace AI;
using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

Vector2 CohesionBehavior::Calculate(Agent& agent)
{
	if (agent.mNeighbors.empty()) {
		return {};
	}

	Vector2 cohesion;

	for (auto& n : agent.mNeighbors) {
		cohesion += n->position;
	}

	cohesion /= agent.mNeighbors.size();

	const auto agentToDest = cohesion - agent.position;
	const float distToDest = Magnitude(agentToDest);
	if (distToDest <= 0.0f)
		return {};

	const auto desiredVelocity = agentToDest / distToDest * agent.maxSpeed;
	const auto cohesionSeekForce = desiredVelocity - agent.velocity;

	if (IsDebug())
	{
		SAGE::DrawScreenDiamond(cohesion, 5.0f, Colors::IndianRed);
		SAGE::DrawScreenLine(agent.position, agent.position + desiredVelocity, Colors::Yellow);
		SAGE::DrawScreenLine(agent.position, agent.position + agent.velocity, Colors::Green);
	}

	return cohesionSeekForce;
}