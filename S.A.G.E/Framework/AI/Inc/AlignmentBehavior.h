#pragma once

#include "SteeringBehavior.h"

namespace AI
{
	class AlignmentBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;
	};
}