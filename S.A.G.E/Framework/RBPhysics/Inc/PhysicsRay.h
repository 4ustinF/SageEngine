#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	struct PhysicsRay
	{
		PhysicsRay() = default;
		PhysicsRay(const Math::Vector3& origin, const Math::Vector3& direction, float maxDistance = std::numeric_limits<float>::max())
			: origin(origin), direction(direction), maxDistance(maxDistance) {
		}

		Math::Vector3 origin = Math::Vector3::Zero;
		Math::Vector3 direction = Math::Vector3::ZAxis;
		float maxDistance = std::numeric_limits<float>::max();

		constexpr Math::Vector3 GetEndPoint() const
		{
			return origin + (direction * maxDistance);
		}

		void DebugDrawRay(Graphics::Color color = Graphics::Colors::Red) const
		{
			Graphics::SimpleDraw::AddLine(origin, GetEndPoint(), color);
		}
	};

	struct PhysicsRayHit
	{
		bool hit = false;
		float distance = 0.0f;
		Math::Vector3 impactPoint;
		Math::Vector3 normal;
		// GameObject?
	};
}