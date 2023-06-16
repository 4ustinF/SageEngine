#include "CloudService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

namespace
{
	const std::string CloudTypeToString(CloudType cloudType)
	{
		switch (cloudType)
		{
			case CloudType::None	: return "None";
			case CloudType::Fast	: return "Fast";
			case CloudType::Fancy	: return "Fancy";
			default	: return "Out of range";
		}
	}
}

void CloudService::Initialize()
{
	SetServiceName("Cloud Service");
	mVoxelCloudsEffect.Initialize();
	mCameraService = GetWorld().GetService<CameraService>();
	
	mCloudsRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCloudsRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mCloudsRenderObject.material.specular = { 0.2f, 0.2f, 0.2f, 1.0f };
	mCloudsRenderObject.material.emissive = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCloudsRenderObject.material.power = 10.0f;
	mCloudsRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/FastCloud.png");
	mCloudsRenderObject.bumpMapId = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/CloudNoise.png");

	mCloudsRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(1, 1, 1.0f, true));
	mCloudsRenderObject.transform.position = { 0.0f, 64.0f, 0.0f };
	mCloudsRenderObject.transform.scale = { 1000.0f, 1.0f, 1000.0f };

	mVoxelCloudsEffect.SetCloudAlpha(mCloudAlpha);
}

void CloudService::Terminate()
{
	mCloudsRenderObject.Terminate();
	mVoxelCloudsEffect.Terminate();
}

void CloudService::Update(float deltaTime)
{
	switch (mCloudType)
	{
	case CloudType::Fast:
	{
		auto& uv = mVoxelCloudsEffect.GetUVs();
		uv.x += cloudXSpeed * cloudSpeedMultiplyer * deltaTime;
		uv.y += -cloudYSpeed * cloudSpeedMultiplyer * deltaTime;
		while (uv.x > 1.0f) { uv.x -= 1.0f; }
		while (uv.x < 0.0f) { uv.x += 1.0f; }
		while (uv.y > 1.0f) { uv.y -= 1.0f; }
		while (uv.y < 0.0f) { uv.y += 1.0f; }
	}
		break;
	case CloudType::Fancy: 
	{
		auto& uv = mVoxelCloudsEffect.GetUVs();
		uv.x += cloudXSpeed * cloudSpeedMultiplyer * deltaTime;
		uv.y += -cloudYSpeed * cloudSpeedMultiplyer * deltaTime;
		while (uv.x > 1.0f) { uv.x -= 1.0f; }
		while (uv.x < 0.0f) { uv.x += 1.0f; }
		while (uv.y > 1.0f) { uv.y -= 1.0f; }
		while (uv.y < 0.0f) { uv.y += 1.0f; }
	}
		break;
	}

	SetCloudPosition(mCameraService->GetCamera().GetPosition());
}

void CloudService::Render()
{
	if (mCloudType == CloudType::None) { return; }

	mVoxelCloudsEffect.SetCamera(mCameraService->GetCamera());

	mVoxelCloudsEffect.Begin();
	mVoxelCloudsEffect.Render(mCloudsRenderObject);
	mVoxelCloudsEffect.End();
}

void CloudService::DebugUI()
{
	ImGui::SliderInt("Cloud Speed X##CloudService", &cloudXSpeed, -10, 10);
	ImGui::SliderInt("Cloud Speed Y##CloudService", &cloudYSpeed, -10, 10);
	ImGui::DragFloat("Cloud Speed Multiplyer##CloudService", &cloudSpeedMultiplyer, 0.00001f, 0.0f, 1.0f, "%.5f");
	if (ImGui::DragFloat("Cloud Alpha##CloudService", &mCloudAlpha, 0.01f, 0.0f, 1.0f, "%.2f"))
	{
		mVoxelCloudsEffect.SetCloudAlpha(mCloudAlpha);
	}

	if (ImGui::Button("Cloud Type##CloudService")) {
		switch (mCloudType) {
		case CloudType::None:	
			mCloudsRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/FastCloud.png");
			mCloudAlpha = 0.5f;
			mVoxelCloudsEffect.UseFast(true);
			mVoxelCloudsEffect.SetCloudFilter(mFilter = Sampler::Filter::Point);
			mCloudType = CloudType::Fast; 
			break;
		case CloudType::Fast:	
			mCloudsRenderObject.diffuseMapId = TextureManager::Get()->LoadTexture("../Sprites/SageCraft/FancyCloud.png");
			mCloudAlpha = 1.0f;
			mVoxelCloudsEffect.UseFast(false);
			mVoxelCloudsEffect.SetCloudFilter(mFilter = Sampler::Filter::Anisotropic);
			mCloudType = CloudType::Fancy; 
			break;
		case CloudType::Fancy:	mCloudType = CloudType::None; break;
		}
		mVoxelCloudsEffect.SetCloudAlpha(mCloudAlpha);
	}
	ImGui::SameLine();
	ImGui::Text(CloudTypeToString(mCloudType).c_str());

	if (ImGui::Button("Cloud Filter##CloudService"))
	{
		switch (mFilter)
		{
		case Sampler::Filter::Point: mFilter = Sampler::Filter::Anisotropic; break;
		case Sampler::Filter::Anisotropic: mFilter = Sampler::Filter::Point; break;
		}

		mVoxelCloudsEffect.SetCloudFilter(mFilter);
	}
}

void CloudService::SetCloudPosition(SAGE::Math::Vector3 position)
{
	mCloudsRenderObject.transform.position = { position.x, 64.0f, position.z};
	//mCloudsRenderObject.transform.position = { position.x, Clamp(position.y + 64.0f, 64.0f, 128.0f), position.z};
}