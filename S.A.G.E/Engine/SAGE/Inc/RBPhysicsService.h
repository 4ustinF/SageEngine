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
		void Render() override;
		void DebugUI() override;

		RBPhysics::RBPhysicsWorld& GetPhysicsWorld() { return mPhysicsWorld; } // TODO: Turn this service more into a wrapper for the physics world and not expose it directly.

		void SetRenderDebugUI(bool renderDebugUI) { mRenderDebugUI = renderDebugUI; }
		void SetFillDebugShapes(bool fillDebugShapes) { mFillDebugShapes = fillDebugShapes; }

	private:
		RBPhysics::RBPhysicsWorld mPhysicsWorld;
		bool mRenderDebugUI = false;
		bool mFillDebugShapes = false;
	};
}