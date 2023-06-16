#pragma once

#include "Service.h"

namespace SAGE
{
	class CameraComponent;

	class CameraService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::Camera)

		void Initialize() override;

		//void SetCamera(const char* gameObjectName);

		Graphics::Camera& GetCamera();
		const Graphics::Camera& GetCamera() const;

	private:
		friend class CameraComponent;
		void Register(CameraComponent* cameraComponent);
		void Unregister(const CameraComponent* cameraComponent);

		std::vector<CameraComponent*> mCameraEntries;
		CameraComponent* mCurrentCamera;
	};
}