#include "Precompiled.h"
#include "TerrainEffect.h"

#include "Camera.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void TerrainEffect::Initialize()
{
	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/Terrain.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Terrain.fx");

	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mSettingsBuffer.Initialize();

	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void TerrainEffect::Terminate()
{
	mSampler.Terminate();

	mSettingsBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mLightBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void TerrainEffect::Begin()
{
	ASSERT(mCamera != nullptr, "TerrainEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "TerrainEffect - No light set");
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
}

void TerrainEffect::End()
{
	if (mShadowMap != nullptr) {
		Texture::UnbindPS(2);
	}
}

void TerrainEffect::Render(const RenderObject& renderObject)
{
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	TransformData transformData;
	transformData.world = Transpose(matWorld);
	transformData.wvp[0] = Transpose(matWorld * view * proj);
	if (mSettingsData.useShadowMap && mShadowMap != nullptr) {
		const auto& matLightView = mLightCamera->GetViewMatrix();
		const auto& matLightProj = mLightCamera->GetProjectionMatrix();
		transformData.wvp[1] = Transpose(matWorld * matLightView * matLightProj);
	}
	transformData.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(transformData);

	mLightBuffer.Update(*mDirectionalLight);
	mMaterialBuffer.Update(renderObject.material);

	SettingsData settingsData;
	if (mSettingsData.useShadowMap == 1 && mShadowMap != nullptr) {
		mShadowMap->BindPS(2);
		settingsData.useShadowMap = 1;
		settingsData.depthBias = mDepthBias;
	}
	else {
		settingsData.useShadowMap = 0;
	}
	mSettingsBuffer.Update(settingsData);

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specularMapId, 1);

	renderObject.meshBuffer.Render();
}

void TerrainEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void TerrainEffect::SetLightCamera(const Camera& camera)
{
	mLightCamera = &camera;
}

void TerrainEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void TerrainEffect::SetShadowMap(const Texture* shadowMap)
{
	mShadowMap = shadowMap;
}

void TerrainEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Standard Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		bool useShadowMap = (mSettingsData.useShadowMap == 1);
		ImGui::Checkbox("Use Shadow Map", &useShadowMap);
		mSettingsData.useShadowMap = useShadowMap ? 1 : 0;

		ImGui::DragFloat("Depth Bias", &mDepthBias, 0.000001f, 0.0f, 1.0f, "%.6f");
	}
}