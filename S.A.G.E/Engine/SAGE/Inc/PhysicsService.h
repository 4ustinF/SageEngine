#pragma once

#include "Service.h"

namespace SAGE
{
	class PhysicsService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::Physics)

		void Initialize() override;
		void Terminate() override;

		void Update(float deltaTime) override;
		void Render() override;
		void DebugUI() override;

		SAGE::Physics::PhysicsWorld& GetPhysicsWorld() { return mPhysicsWorld; }
		const SAGE::Physics::PhysicsWorld& GetPhysicsWorld() const { return mPhysicsWorld; }

	private:
		SAGE::Physics::PhysicsWorld mPhysicsWorld;
	};
}