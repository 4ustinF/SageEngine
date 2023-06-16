#pragma once

#include "Common.h"

namespace SAGE::Physics
{
	struct RigidAABB
	{
		Math::AABB aabb;
		Math::Vector3 lastPosition = Math::Vector3::Zero;
		Math::Vector3 acceleration = Math::Vector3::Zero;
		Math::Vector3 force = Math::Vector3::Zero;
		float invMass = 0.0f;

		RigidAABB() = default;
		RigidAABB(Math::AABB aabb, float invMass)
			: aabb(aabb), invMass(invMass)
		{}

		void SetPosition(const Math::Vector3& pos) {
			aabb.center = pos;
			lastPosition = pos;
		}

		void SetVelocity(const Math::Vector3& velocity) {
			lastPosition = aabb.center - velocity;
		}

		const Math::Vector3 GetVelocity() const {
			return aabb.center - lastPosition;
		}
	};
}