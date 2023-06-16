#include "Precompiled.h"
#include "CrossHatchEffect.h"

#include "Camera.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void CrossHatchEffect::Initialize()
{
	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/CrossHatchShader.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/CrossHatchShader.fx");

	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mSettingsBuffer.Initialize();
	mMaterialBuffer.Initialize();

	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void CrossHatchEffect::Terminate()
{
	mSampler.Terminate();

	mMaterialBuffer.Terminate();
	mSettingsBuffer.Terminate();
	mLightBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();

	mHatchMapDark = 0;
	mHatchMapBright = 0;
}

void CrossHatchEffect::Begin()
{
	ASSERT(mCamera != nullptr, "CrossHatchEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "CrossHatchEffect - No light set");
	ASSERT(mHatchMapDark != 0, "CrossHatchEffect - mHatchMapDark not set");
	ASSERT(mHatchMapBright != 0, "CrossHatchEffect - mHatchMapBright not set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mLightBuffer.BindVS(1);
	mLightBuffer.BindPS(1);

	mSettingsBuffer.BindVS(2);
	mSettingsBuffer.BindPS(2);

	mMaterialBuffer.BindPS(3);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void CrossHatchEffect::End()
{
}

void CrossHatchEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) {
		Render(renderObjects);
	}
}

void CrossHatchEffect::Render(const RenderObject& renderObject)
{
	const auto& matWorld = renderObject.transform.GetMatrix4();
	const auto& view = mCamera->GetViewMatrix();
	const auto& proj = mCamera->GetProjectionMatrix();

	TransformData transformData;
	transformData.world = Transpose(matWorld);
	transformData.wvp[0] = Transpose(matWorld * view * proj);
	transformData.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(transformData);

	mLightBuffer.Update(*mDirectionalLight);

	SettingsData settingsData;
	settingsData.param0 = mIntensity;
	if (mSettingsData.useDiffuseMap == 0 || renderObject.diffuseMapId == 0) {
		settingsData.useDiffuseMap = 0;
	}
	if (mSettingsData.useSpecularMap == 0 || renderObject.specularMapId == 0) {
		settingsData.useSpecularMap = 0;
	}
	if (mSettingsData.useNormalMap == 0 || renderObject.normalMapId == 0) {
		settingsData.useNormalMap = 0;
	}
	mSettingsBuffer.Update(settingsData);

	mMaterialBuffer.Update(renderObject.material);

	auto tm = TextureManager::Get();
	tm->BindPS(mHatchMapDark, 0);
	tm->BindPS(mHatchMapBright, 1);
	tm->BindPS(renderObject.diffuseMapId, 2);
	tm->BindPS(renderObject.specularMapId, 3);
	tm->BindPS(renderObject.normalMapId, 4);

	renderObject.meshBuffer.Render();
}

void CrossHatchEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void CrossHatchEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void CrossHatchEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Cross Hatch Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		bool useDiffuseMap = (mSettingsData.useDiffuseMap == 1);
		ImGui::Checkbox("Use Diffuse Map##CrossHatchEffect", &useDiffuseMap);
		mSettingsData.useDiffuseMap = useDiffuseMap ? 1 : 0;

		bool useSpecularMap = (mSettingsData.useSpecularMap == 1);
		ImGui::Checkbox("Use Specular Map##StandardEffect", &useSpecularMap);
		mSettingsData.useSpecularMap = useSpecularMap ? 1 : 0;

		bool useNormalMap = (mSettingsData.useNormalMap == 1);
		ImGui::Checkbox("Use Normal Map##StandardEffect", &useNormalMap);
		mSettingsData.useNormalMap = useNormalMap ? 1 : 0;

		ImGui::DragFloat("Intensity multiplier", &mIntensity, 0.001f, 0.0f, 10.0f);
	}
}