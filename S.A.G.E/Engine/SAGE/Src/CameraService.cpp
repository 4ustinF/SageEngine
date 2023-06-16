#include "Precompiled.h"
#include "CameraService.h"

#include "CameraComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void CameraService::Initialize()
{
	SetServiceName("Camera Service");
}

Camera& CameraService::GetCamera()
{
	return mCurrentCamera->GetCamera();
}

const Camera& CameraService::GetCamera() const
{
	return mCurrentCamera->GetCamera();
}

void CameraService::Register(CameraComponent* cameraComponent)
{
	mCameraEntries.push_back(cameraComponent);

	if (mCurrentCamera == nullptr) {
		mCurrentCamera = cameraComponent;
	}
}

void CameraService::Unregister(const CameraComponent* cameraComponent)
{
	auto iter = std::find(mCameraEntries.begin(), mCameraEntries.end(), cameraComponent);
	if (iter != mCameraEntries.end())
	{
		if (mCurrentCamera == *iter)
		{
			if (mCameraEntries.empty()) {
				mCurrentCamera = nullptr;
			}
			else {
				mCurrentCamera = mCameraEntries.front();
			}
		}
		mCameraEntries.erase(iter);
	}
}