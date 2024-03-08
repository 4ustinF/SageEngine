#pragma once
#include "SteeringBehavior.h"

namespace SAGE::AI
{
	class FleeBehavior : public SteeringBehavior
	{
	public:
		SAGE::Math::Vector2 Calculate(Agent& agent) override;

		void SetPanicDistance(const float dist) { panicDistance = dist; }
		const bool GetIsFleeing() const { return isFleeing; }

	private:
		float panicDistance = 100.0f;
		bool isFleeing = false;
	};
}