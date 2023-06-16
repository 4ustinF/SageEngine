#include "Precompiled.h"
#include "VoxelCloudsEffect.h"

#include "RenderObject.h"
#include "VertexTypes.h"
#include "Camera.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void VoxelCloudsEffect::Initialize()
{
	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/VoxelClouds.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/VoxelClouds.fx");

	mTransformBuffer.Initialize();
	mSettingsBuffer.Initialize();
	mSkyBuffer.Initialize();

	mAlphaBlendState.Initialize(BlendState::Mode::AlphaBlend);
	mSampler.Initialize(Sampler::Filter::Point, Sampler::AddressMode::Wrap);
}

void VoxelCloudsEffect::Terminate()
{
	mSampler.Terminate();
	mAlphaBlendState.Terminate();

	mSkyBuffer.Terminate();
	mSettingsBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void VoxelCloudsEffect::Begin()
{
	ASSERT(mCamera != nullptr, "VoxelCloudsEffect - No camera set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mSettingsBuffer.BindPS(1);
	mSkyBuffer.BindPS(2);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void VoxelCloudsEffect::End()
{
}

void VoxelCloudsEffect::Render(const RenderObject& renderObject)
{
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	mTransformBuffer.Update(Transpose(matWorld * view * proj));

	SettingsData settingsData;
	settingsData.useFast = mUseFast;
	mSettingsBuffer.Update(settingsData);

	SkyData skydata;
	skydata.translation = mUVs;
	skydata.scale = 0.3f;
	skydata.brightness = mCloudAlpha;
	mSkyBuffer.Update(skydata);

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.bumpMapId, 1);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void VoxelCloudsEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void VoxelCloudsEffect::SetCloudFilter(const Sampler::Filter& filter)
{
	mSampler.Initialize(filter, Sampler::AddressMode::Wrap);
}