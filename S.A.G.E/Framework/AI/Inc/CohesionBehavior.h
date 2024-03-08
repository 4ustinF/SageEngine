#pragma once

#include "SteeringBehavior.h"

namespace SAGE::AI
{
	class CohesionBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;
	};
}