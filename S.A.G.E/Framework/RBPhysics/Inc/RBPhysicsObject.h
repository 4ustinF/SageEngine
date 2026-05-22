#pragma once

#include "Common.h"

namespace SAGE::RBPhysics
{
	class RBPhysicsObject
	{
	public:
		RBPhysicsObject() = default;
		RBPhysicsObject(Math::Vector3 position, Math::Vector3 velocity)
			: mPosition(position), mVelocity(velocity)
		{
		}

		void Integrate(float deltaTime);

		const Math::Vector3& GetPosition() const { return mPosition; }
		const Math::Vector3& GetVelocity() const { return mVelocity; }

	private:
		Math::Vector3 mPosition = Math::Vector3::Zero;
		Math::Vector3 mVelocity = Math::Vector3::Zero;
	};
}