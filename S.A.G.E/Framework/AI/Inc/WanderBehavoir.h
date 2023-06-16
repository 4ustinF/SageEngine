#pragma once

#include "SteeringBehavior.h"

namespace AI
{
	class WanderBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;

		void Setup(float radius, float distance, float jitter);

	private:
		SAGE::Math::Vector2 mLocalWanderTarget = SAGE::Math::Vector2::YAxis;
		float mWanderRadius = 1.0f;
		float mWanderDistance = 0.0f;
		float mWanderJitter = 0.0f;
	};
}