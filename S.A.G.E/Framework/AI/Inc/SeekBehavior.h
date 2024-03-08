#pragma once

#include "SteeringBehavior.h"

namespace SAGE::AI
{
	class SeekBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;
	};
}