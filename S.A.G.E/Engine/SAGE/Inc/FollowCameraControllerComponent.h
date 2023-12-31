#pragma once

#include "Component.h"

namespace SAGE
{
	class CameraComponent;

	class FollowCameraControllerComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::FollowCameraController)
		MEMORY_POOL_DECLARE

		void Initialize() override;
		void Update(float deltaTime) override;

		void SetTargetName(const char* name) { mTargetName = name; }
		void SetDistance(float distance) { mDistance = distance; }
		void SetCameraElevation(float elevation) { mCameraElevation = elevation; }
		void SetLookElevation(float elevation) { mLookElevation = elevation; }

	private:
		CameraComponent* mCameraComponent = nullptr;
		std::string mTargetName = "Player";
		float mDistance = 1.0f;
		float mCameraElevation = 1.0f;
		float mLookElevation = 1.0f;
	};
}