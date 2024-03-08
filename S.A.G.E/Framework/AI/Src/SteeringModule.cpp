#include "Precompiled.h"
#include "SteeringModule.h"

using namespace SAGE::AI;
using namespace SAGE;
using namespace SAGE::Math;

SteeringModule::SteeringModule(Agent& agent)
	: mAgent(agent)
{
}

Vector2 SteeringModule::Calculate()
{
	Vector2 totalForce;

	for (auto& behavior : mBehaviors)
	{
		if (behavior->IsActive())
			totalForce += behavior->Calculate(mAgent) * behavior->GetWeight();
	}

	return totalForce;
}