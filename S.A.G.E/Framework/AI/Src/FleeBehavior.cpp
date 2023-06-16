#include "Precompiled.h"
#include "FleeBehavior.h"

#include "Agent.h"
using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

using namespace AI;

Vector2 FleeBehavior::Calculate(Agent& agent)
{
	if (agent.target == nullptr) {
		return{};
	}
	
	const auto agentFromDest = agent.position - agent.target->position;
	const float distAwayDest = Magnitude(agentFromDest);
	if (distAwayDest <= 0.0f || distAwayDest > panicDistance) {
		isFleeing = false;
		return {};
	}
		
	const auto desiredVelocity = (agentFromDest / distAwayDest) * agent.maxSpeed;
	const auto seekForce = desiredVelocity - agent.velocity;

	if (IsDebug()) {
		SAGE::DrawScreenLine(agent.position, agent.position + desiredVelocity, Colors::Yellow);
		SAGE::DrawScreenLine(agent.position, agent.position + agent.velocity, Colors::Green);

		SAGE::DrawScreenCircle(agent.position, panicDistance, Color(Colors::Blue));
	}

	isFleeing = true;
	return seekForce;
}