#pragma once

namespace SAGE::CameraUtil
{
	void UpdateFPSCamera(SAGE::Graphics::Camera& camera, float normSpeed, float fastSpeed, float turnSpeed, float deltaTime, bool useDebugMode = true);
}