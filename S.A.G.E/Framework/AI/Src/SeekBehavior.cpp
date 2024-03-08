#include "Precompiled.h"
#include "SeekBehavior.h"

#include "Agent.h"

using namespace SAGE::AI;
using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

Vector2 SeekBehavior::Calculate(Agent& agent)
{
	const auto agentToDest = agent.destination - agent.position;
	const float distToDest = Magnitude(agentToDest);
	if (distToDest <= 0.0f)
		return {};

	const auto desiredVelocity = (agentToDest / distToDest) * agent.maxSpeed;
	const auto seekForce = desiredVelocity - agent.velocity;

	if (IsDebug())
	{
		//SAGE::DrawScreenLine(agent.position, agent.position + desiredVelocity, Colors::Yellow);
		//SAGE::DrawScreenLine(agent.position, agent.position + agent.velocity, Colors::Green);
	}

	return seekForce;
}