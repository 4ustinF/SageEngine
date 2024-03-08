#include "Precompiled.h"
#include "WanderBehavoir.h"

#include "Agent.h"

using namespace SAGE::AI;
using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

Vector2 WanderBehavior::Calculate(Agent& agent)
{
	//Applied random jitter to wander target
	auto newWanderTarget = mLocalWanderTarget + Random::OnUnitCircle() * mWanderJitter;
	newWanderTarget = Normalize(newWanderTarget) * mWanderRadius;
	mLocalWanderTarget = newWanderTarget;

	//Project the new position infront of the agent
	newWanderTarget += Vector2(0.0f, mWanderDistance);

	//Transform target to world space
	const auto worldTransform = agent.GetWorldTransform();
	const auto worldWanderTarget = TransformCoord(newWanderTarget, worldTransform);

	const auto agentToDest = worldWanderTarget - agent.position;
	const float distToDest = Magnitude(agentToDest);
	if (distToDest <= 0.0f)
		return {};

	//Seek to wander target
	const auto desiredVelocity = (agentToDest / distToDest) * agent.maxSpeed;
	const auto seekForce = desiredVelocity - agent.velocity;

	if (IsDebug())
	{
		const auto wanderCenter = TransformCoord({ 0.0f, mWanderDistance }, worldTransform);
		//SAGE::DrawScreenCircle(wanderCenter, mWanderRadius, Colors::Green);
		//
		//SAGE::DrawScreenDiamond(worldWanderTarget, 3.0f, Colors::Red);
		//SAGE::DrawScreenLine(agent.position, worldWanderTarget, Colors::Yellow);
	}

	return seekForce;
}

void WanderBehavior::Setup(float radius, float distance, float jitter)
{
	mWanderRadius = radius;
	mWanderDistance = distance;
	mWanderJitter = jitter;
}