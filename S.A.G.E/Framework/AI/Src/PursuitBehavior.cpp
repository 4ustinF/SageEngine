#include "Precompiled.h"
#include "PursuitBehavior.h"

#include "Agent.h"
using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

using namespace AI;

Vector2 PursuitBehavior::Calculate(Agent& agent)
{
	if (agent.target == nullptr) {
		return{};
	}

	const auto& target = agent.target;

	const auto agentToTargetDest = target->position - agent.position;
	const float distance = Magnitude(agentToTargetDest);

	if (distance > chaseDistance) {
		agent.destination = target->position + target->velocity;
	}
	else {
		agent.destination = target->position;
	}

	//Have to get velocity of the target to predict where its going
	//Grab targets location and velocity and guess where it will be in x time and set that destination as your destination

	//We can optimize this by sitting the agents destination to the targets location if they are heading towards the pursuer

	const auto agentToDest = agent.destination - agent.position;
	const float distToDest = Magnitude(agentToDest);
	if (distToDest <= 0.0f)
		return {};

	const auto desiredVelocity = (agentToDest / distToDest) * agent.maxSpeed;
	const auto seekForce = desiredVelocity - agent.velocity;

	if (IsDebug())
	{
		SAGE::DrawScreenLine(agent.position, agent.position + desiredVelocity, Colors::Yellow);
		SAGE::DrawScreenLine(agent.position, agent.position + agent.velocity, Colors::Green);
		SAGE::DrawScreenCircle(agent.position, chaseDistance, Color(Colors::Red));
	}

	return seekForce;
}