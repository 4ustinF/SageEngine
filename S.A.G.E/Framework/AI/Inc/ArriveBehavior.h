#pragma once

#include "SteeringBehavior.h"

namespace AI
{
	class ArriveBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;

		void SetArrivalDistance(const float dist) { arrivalDistance = dist; }

	private:
		float arrivalDistance = 100.0f;
	};
}