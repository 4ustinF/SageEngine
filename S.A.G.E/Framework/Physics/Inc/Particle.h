#pragma once

#include "Common.h"

namespace SAGE::Physics
{
	struct Particle // Verlet Particle
	{
		Math::Vector3 position = Math::Vector3::Zero;
		Math::Vector3 lastPosition = Math::Vector3::Zero;
		Math::Vector3 acceleration = Math::Vector3::Zero;
		float radius = 1.0f;
		float invMass = 0.0f; 
		float bounce = 1.0f;

		Particle() = default;
		Particle(float radius, float invMass)
			: radius(radius), invMass(invMass)
		{}

		void SetPosition(const Math::Vector3& pos)
		{
			position = pos;
			lastPosition = pos;
		}

		void SetVelocity(const Math::Vector3& velocity)
		{
			lastPosition = position - velocity;
		}
	};
}