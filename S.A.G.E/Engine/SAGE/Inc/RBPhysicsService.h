#pragma once

#include "Service.h"

namespace SAGE
{
	class RBPhysicsService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::RBPhysics)

		void Initialize() override;
		void Terminate() override;
		void Update(float deltaTime) override;
		void DebugUI() override;

	private:

		SAGE::RBPhysics::RBPhysicsWorld mPhysicsWorld;
		bool mRenderDebugUI = false;
	};
}