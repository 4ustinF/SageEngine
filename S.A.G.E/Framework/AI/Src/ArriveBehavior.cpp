#include "Precompiled.h"
#include "ArriveBehavior.h"

#include "Agent.h"

using namespace AI;
using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

Vector2 ArriveBehavior::Calculate(Agent& agent)
{
	const auto agentToDest = agent.destination - agent.position;
	const float distToDest = Magnitude(agentToDest);

	if (distToDest <= 1.0f) {
		agent.velocity = Vector2::Zero;
		return{};
	}

	float speed = agent.maxSpeed;
	if (distToDest <= arrivalDistance) {
		speed = (agent.maxSpeed * (distToDest / arrivalDistance));
	}

	const auto desiredVelocity = (agentToDest / distToDest) * speed;

	if (distToDest <= arrivalDistance) {
		agent.velocity = desiredVelocity;
	}
	
	const auto seekForce = desiredVelocity - agent.velocity;

	if (IsDebug())
	{
		SAGE::DrawScreenLine(agent.position, agent.position + desiredVelocity, Colors::Yellow);
		SAGE::DrawScreenLine(agent.position, agent.position + agent.velocity, Colors::Green);

		//Agent Velocity
		std::string str = std::to_string(agent.velocity.x).c_str();
		str += " ";
		str += std::to_string(agent.velocity.y).c_str();

		Font::Get()->Draw(str.c_str(), agent.position + 5.0f, 16.0f, Colors::White);

		//Desired Velocity
		str = std::to_string(desiredVelocity.x).c_str();
		str += " ";
		str += std::to_string(desiredVelocity.y).c_str();

		Font::Get()->Draw(str.c_str(), agent.position + 25.0f, 16.0f, Colors::Yellow);
	}

	return seekForce;
}
