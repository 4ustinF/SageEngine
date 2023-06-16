#pragma once

#include "SteeringBehavior.h"

namespace AI
{
	class SeekBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;
	};
}