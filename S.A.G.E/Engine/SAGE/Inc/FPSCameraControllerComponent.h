#pragma once

#include "Component.h"

namespace SAGE
{
	class CameraComponent;

	class FPSCameraControllerComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::FPSCameraController)
		MEMORY_POOL_DECLARE

		void Initialize() override;
		void Update(float deltaTime) override;

		void DebugUI() override;

		void SetNormSpeed(float speed) { normSpeed = speed; }
		void SetFastSpeed(float speed) { fastSpeed = speed; }
		void SetTurnSpeed(float speed) { turnSpeed = speed; }

		void SetDebugKey(SAGE::Input::KeyCode keyCode) { mDebugKeyCode = keyCode; }

	private:
		CameraComponent* mCameraComponent = nullptr;
		float normSpeed = 2.0f;
		float fastSpeed = 5.0f;
		float turnSpeed = 0.1f;

		bool useMouseDebugMode = true;
		SAGE::Input::KeyCode mDebugKeyCode = SAGE::Input::KeyCode::M;
	};
}