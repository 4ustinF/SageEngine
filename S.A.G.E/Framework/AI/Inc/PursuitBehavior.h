#pragma once

#include "SteeringBehavior.h"

namespace AI
{
	class PursuitBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;

		void SetChaseDistance(const float dist) { chaseDistance = dist; }

	private:
		float chaseDistance = 100.0f;
	};
}