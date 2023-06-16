#pragma once

#include "TypeIds.h"
#include "Enums.h"

class CloudService final : public SAGE::Service
{
public:
	SET_TYPE_ID(ServiceId::Clouds);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

	void SetCloudPosition(SAGE::Math::Vector3 position);

private:
	SAGE::Graphics::RenderObject mCloudsRenderObject;
	SAGE::Graphics::VoxelCloudsEffect mVoxelCloudsEffect;

	const SAGE::CameraService* mCameraService = nullptr;

	int cloudXSpeed = 10;
	int cloudYSpeed = -10;
	float cloudSpeedMultiplyer = 0.00025f;
	float mCloudAlpha = 0.5f;

	SAGE::Graphics::Sampler::Filter mFilter = SAGE::Graphics::Sampler::Filter::Point;
	CloudType mCloudType = CloudType::Fast;
};