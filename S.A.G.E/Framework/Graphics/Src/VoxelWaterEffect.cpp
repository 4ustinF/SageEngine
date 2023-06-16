#include "Precompiled.h"
#include "VoxelWaterEffect.h"

#include "Camera.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void VoxelWaterEffect::Initialize(Sampler::Filter sampleFilter)
{
	mVertexShader.Initialize<VoxelVertex>(L"../../Assets/Shaders/VoxelWater.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/VoxelWater.fx");

	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mSettingsBuffer.Initialize();

	mAlphaBlendState.Initialize(mBlendStateMode);
	mSampler.Initialize(sampleFilter, Sampler::AddressMode::Wrap);
}

void VoxelWaterEffect::Terminate()
{
	mSampler.Terminate();

	mAlphaBlendState.Terminate();

	mSettingsBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mLightBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void VoxelWaterEffect::Begin()
{
	ASSERT(mCamera != nullptr, "VoxelWaterEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "VoxelWaterEffect - No light set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mLightBuffer.BindVS(1);
	mLightBuffer.BindPS(1);

	mMaterialBuffer.BindPS(2);

	mSettingsBuffer.BindVS(3);
	mSettingsBuffer.BindPS(3);

	mSampler.BindVS(0);
	mSampler.BindPS(0);

	mFrequency += isIncrementing ? 0.0005f : -0.0005f;
	if (mFrequency >= 10.5f) { isIncrementing = false; }
	if (mFrequency <= 0.5f) { isIncrementing = true; }
}

void VoxelWaterEffect::End()
{
}

void VoxelWaterEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) {
		Render(renderObjects);
	}
}

void VoxelWaterEffect::Render(const RenderObject& renderObject)
{
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	TransformData transformData;
	transformData.world = Transpose(matWorld);
	transformData.wvp[0] = Transpose(matWorld * view * proj);
	transformData.viewPosition = mCamera->GetPosition();
	transformData.chunkSize = mChunkSize;
	mTransformBuffer.Update(transformData);

	mLightBuffer.Update(*mDirectionalLight);
	mMaterialBuffer.Update(renderObject.material);

	SettingsData settingsData;
	if (mSettingsData.useFog == 1) {
		settingsData.useFog = 1;
		settingsData.fogColor = mFogColor;
		settingsData.fogStart = mFogStart;
		settingsData.fogEnd = mFogEnd;
	}

	settingsData.deltaTime = mDeltaTime;
	settingsData.amplitude = mAmplitude;
	settingsData.frequency = mFrequency;
	settingsData.waveLength = mWaveLength;
	mSettingsBuffer.Update(settingsData);

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specularMapId, 1);
	tm->BindPS(renderObject.normalMapId, 2);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void VoxelWaterEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void VoxelWaterEffect::SetLightCamera(const Camera& camera)
{
	mLightCamera = &camera;
}

void VoxelWaterEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void VoxelWaterEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Voxel Water Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::InputFloat("Amplitude##VoxelWaterEffect", &mAmplitude, 0.01f, 0.05f);
		ImGui::InputFloat("Wave Length##VoxelWaterEffect", &mWaveLength, 0.01f, 0.05f);

		bool useFog = (mSettingsData.useFog == 1);
		ImGui::Checkbox("Use Fog##VoxelWaterEffect", &useFog);
		mSettingsData.useFog = useFog ? 1 : 0;

		ImGui::DragFloat("Fog Start##VoxelWaterEffect", &mFogStart, 1.0f, 0.0f, 250.0f);
		ImGui::DragFloat("Fog End##VoxelWaterEffect", &mFogEnd, 1.0f, 0.0f, 250.0f);
		ImGui::ColorEdit4("Fog Color##VoxelWaterEffect", &mFogColor.r);
	}
}