#include "Precompiled.h"
#include "ToonEffect.h"

#include "Camera.h"
#include "RenderObject.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void ToonEffect::Initialize()
{
	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/ToonShader.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/ToonShader.fx");

	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mSettingsBuffer.Initialize();

	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void ToonEffect::Terminate()
{
	mSampler.Terminate();

	mSettingsBuffer.Terminate();
	mLightBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void ToonEffect::Begin()
{
	ASSERT(mCamera != nullptr, "ToonEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "ToonEffect - No light set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mLightBuffer.BindVS(1);
	mLightBuffer.BindPS(1);

	mSettingsBuffer.BindVS(2);
	mSettingsBuffer.BindPS(2);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void ToonEffect::End()
{

}

void ToonEffect::Render(const RenderObject& renderObject)
{
	//ASSERT(mCelmap, "ToonEffect --- mCelmap is null!");
	auto tm = TextureManager::Get();

	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	TransformData transformData;
	transformData.world = Transpose(matWorld);
	transformData.wvp = Transpose(matWorld * view * proj);
	transformData.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(transformData);

	mLightBuffer.Update(*mDirectionalLight);

	SettingsData settingsData;
	if (mSettingsData.useDiffuseMap == 0 || renderObject.diffuseMapId == 0) {
		settingsData.useDiffuseMap = 0;
	}
	if (mSettingsData.useCelMap == 1) {
		settingsData.useCelMap = 1;
		tm->BindPS(*mCelmap, 1);
	}
	if (mSettingsData.useRim == 0) {
		settingsData.useRim = 0;
	}
	settingsData.rimIntensity = mRimIntensity;
	mSettingsBuffer.Update(settingsData);

	tm->BindPS(renderObject.diffuseMapId, 0);

	renderObject.meshBuffer.Render();
}

void ToonEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void ToonEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void ToonEffect::SetCelMap(const TextureId* celMap)
{
	mCelmap = celMap;
}

void ToonEffect::SetRimIntensity(float intensity)
{
	if (intensity > 1.0f) { intensity = 1.0f; }
	if (intensity < 0.0f) { intensity = 0.0f; }

	mRimIntensity = intensity;
}

void ToonEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Toon Effect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool useDiffuseMap = (mSettingsData.useDiffuseMap == 1);
		ImGui::Checkbox("Use Diffuse Map##ToonEffect", &useDiffuseMap);
		mSettingsData.useDiffuseMap = useDiffuseMap ? 1 : 0;

		bool useRim = (mSettingsData.useRim == 1);
		ImGui::Checkbox("Use Rim##ToonEffect", &useRim);
		mSettingsData.useRim = useRim ? 1 : 0;

		ImGui::DragFloat("Rim Intensity##ToonEffect", &mRimIntensity, 0.01f, 0.0f, 1.0f);
	}
}