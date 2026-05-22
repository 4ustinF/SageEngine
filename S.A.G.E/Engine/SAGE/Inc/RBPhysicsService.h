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

		void SetGravity(float gravity);
		void SetMaxSubSteps(int steps) { mMaxSubSteps = steps; }

	private:

		bool mRenderDebugUI = false;
		float mGravity = -9.81f;
		int mMaxSubSteps = 1;
	};
}