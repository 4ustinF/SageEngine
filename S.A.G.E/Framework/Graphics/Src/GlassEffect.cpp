#include "Precompiled.h"
#include "GlassEffect.h"

#include "AnimationUtil.h"
#include "Animator.h"
#include "Camera.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

void GlassEffect::Initialize(Sampler::Filter sampleFilter)
{
	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/Glass.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Glass.fx");

	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mSettingsBuffer.Initialize();
	mGlassBuffer.Initialize();

	mAlphaBlendState.Initialize(mBlendStateMode);
	mSampler.Initialize(sampleFilter, Sampler::AddressMode::Wrap);
}

void GlassEffect::Terminate()
{
	mSampler.Terminate();

	mAlphaBlendState.Terminate();

	mGlassBuffer.Terminate();
	mSettingsBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mLightBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void GlassEffect::Begin()
{
	ASSERT(mCamera != nullptr, "GlassEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "GlassEffect - No light set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mLightBuffer.BindVS(1);
	mLightBuffer.BindPS(1);

	mMaterialBuffer.BindPS(2);

	mSettingsBuffer.BindVS(3);
	mSettingsBuffer.BindPS(3);

	mGlassBuffer.BindPS(4);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void GlassEffect::End()
{
	if (mShadowMap != nullptr) {
		Texture::UnbindPS(4);
	}
}

void GlassEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) {
		Render(renderObjects);
	}
}

void GlassEffect::Render(const RenderObject& renderObject)
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
	transformData.bumpWeight = mBumpWeight;
	mTransformBuffer.Update(transformData);

	mLightBuffer.Update(*mDirectionalLight);
	mMaterialBuffer.Update(renderObject.material);

	SettingsData settingsData;
	if (mSettingsData.useDiffuseMap == 0 || renderObject.diffuseMapId == 0) {
		settingsData.useDiffuseMap = 0;
	}
	if (mSettingsData.useSpecularMap == 0 || renderObject.specularMapId == 0) {
		settingsData.useSpecularMap = 0;
	}
	if (mSettingsData.useBumpMap == 0 || renderObject.bumpMapId == 0) {
		settingsData.useBumpMap = 0;
	}
	if (mSettingsData.useNormalMap == 0 || renderObject.normalMapId == 0) {
		settingsData.useNormalMap = 0;
	}
	if (mSettingsData.useFog == 1) {
		settingsData.useFog = 1;
		settingsData.fogColor = mFogColor;
		settingsData.fogStart = mFogStart;
		settingsData.fogEnd = mFogEnd;
	}
	if (mSettingsData.useShadowMap == 1 && mShadowMap != nullptr) {
		mShadowMap->BindPS(4);
		settingsData.useShadowMap = 1;
		settingsData.depthBias = mDepthBias;
		settingsData.sampleSize = mSampleSize;
	}
	else {
		settingsData.useShadowMap = 0;
	}

	settingsData.tiling = renderObject.tilingSize;
	settingsData.tilingOffset = renderObject.tilingOffset;

	mSettingsBuffer.Update(settingsData);

	auto tm = TextureManager::Get();

	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specularMapId, 1);
	tm->BindVS(renderObject.bumpMapId, 2);
	tm->BindPS(renderObject.normalMapId, 3);

	tm->BindPS(mShatterMapId, 5);
	tm->BindPS(mShatterNormalMapId, 6);

	GlassData glassData = mGlassData;

	if (mShatterMapId == 0)
	{
		glassData.useShatterMap = 0;
	}

	if (mShatterNormalMapId == 0)
	{
		glassData.useShatterNormalMap = 0;
	}

	glassData.impactCount = std::min(glassData.impactCount, MaxGlassImpacts);
	mGlassBuffer.Update(glassData);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void GlassEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void GlassEffect::SetLightCamera(const Camera& camera)
{
	mLightCamera = &camera;
}

void GlassEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void GlassEffect::SetShadowMap(const Texture* shadowMap)
{
	mShadowMap = shadowMap;
}

void GlassEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Glass Effect##GlassEffect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Bump Weight##GlassEffect", &mBumpWeight, 0.01f, 0.0f, 5.0f);

		bool useDiffuseMap = (mSettingsData.useDiffuseMap == 1);
		ImGui::Checkbox("Use Diffuse Map##GlassEffect", &useDiffuseMap);
		mSettingsData.useDiffuseMap = useDiffuseMap ? 1 : 0;

		bool useSpecularMap = (mSettingsData.useSpecularMap == 1);
		ImGui::Checkbox("Use Specular Map##GlassEffect", &useSpecularMap);
		mSettingsData.useSpecularMap = useSpecularMap ? 1 : 0;

		bool useBumpMap = (mSettingsData.useBumpMap == 1);
		ImGui::Checkbox("Use Bump Map##GlassEffect", &useBumpMap);
		mSettingsData.useBumpMap = useBumpMap ? 1 : 0;

		bool useNormalMap = (mSettingsData.useNormalMap == 1);
		ImGui::Checkbox("Use Normal Map##GlassEffect", &useNormalMap);
		mSettingsData.useNormalMap = useNormalMap ? 1 : 0;

		bool useShadowMap = (mSettingsData.useShadowMap == 1);
		ImGui::Checkbox("Use Shadow Map", &useShadowMap);
		mSettingsData.useShadowMap = useShadowMap ? 1 : 0;

		bool useFog = (mSettingsData.useFog == 1);
		ImGui::Checkbox("Use Fog##GlassEffect", &useFog);
		mSettingsData.useFog = useFog ? 1 : 0;

		ImGui::DragFloat("Fog Start##GlassEffect", &mFogStart, 1.0f, 0.0f, 250.0f);
		ImGui::DragFloat("Fog End##GlassEffect", &mFogEnd, 1.0f, 0.0f, 250.0f);
		ImGui::ColorEdit4("Fog Color##FogEffect", &mFogColor.r);

		ImGui::DragFloat("Depth Bias##GlassEffect", &mDepthBias, 0.000001f, 0.0f, 1.0f, "%.6f");
		ImGui::DragInt("Sample Size##GlassEffect", &mSampleSize, 0.1f, 0, 5);

		bool useShatterMap = (mGlassData.useShatterMap == 1);
		ImGui::Checkbox("Use Shatter Map##GlassEffect", &useShatterMap);
		mGlassData.useShatterMap = useShatterMap ? 1 : 0;

		bool useShatterNormalMap = (mGlassData.useShatterNormalMap == 1);
		ImGui::Checkbox("Use Shatter Normal Map##GlassEffect", &useShatterNormalMap);
		mGlassData.useShatterNormalMap = useShatterNormalMap ? 1 : 0;

		ImGui::DragFloat("Shatter Intensity##GlassEffect", &mGlassData.shatterIntensity, 0.01f, 0.0f, 5.0f);
		ImGui::ColorEdit4("Shatter Color##GlassEffect", &mGlassData.shatterColor.r);

		ImGui::Text("Impact Count: %d", mGlassData.impactCount);

		// TODO: Clean debug code up
		static SAGE::Math::Vector2 testUV = { 0.5f, 0.5f };
		static float testRadius = 0.15f;
		static float testStrength = 1.0f;

		ImGui::DragFloat2("Impact UV##GlassEffect", &testUV.x, 0.001f, 0.0f, 1.0f);

		ImGui::DragFloat("Impact Radius##GlassEffect", &testRadius, 0.001f, 0.01f, 1.0f);

		ImGui::DragFloat("Impact Strength##GlassEffect", &testStrength, 0.01f, 0.0f, 5.0f);

		if (ImGui::Button("Add Impact##GlassEffect"))
		{
			AddImpact(testUV, testRadius, testStrength, 0.0f);
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear Impacts##GlassEffect"))
		{
			ClearImpacts();
		}
	}
}

void GlassEffect::ClearImpacts()
{
	mGlassData.impactCount = 0;

	for (int i = 0; i < MaxGlassImpacts; ++i)
	{
		mGlassData.impacts[i] = {};
	}
}

void GlassEffect::AddImpact(const Math::Vector2& uv, float radius, float strength, float rotation)
{
	if (mGlassData.impactCount >= MaxGlassImpacts)
	{
		// Simple behavior for now: overwrite the oldest/first impact.
		// You can change this later to rotate through them.
		for (int i = 1; i < MaxGlassImpacts; ++i)
		{
			mGlassData.impacts[i - 1] = mGlassData.impacts[i];
		}

		mGlassData.impactCount = MaxGlassImpacts - 1;
	}

	GlassImpactData& impact = mGlassData.impacts[mGlassData.impactCount];
	impact.uv = uv;
	impact.radius = radius;
	impact.strength = strength;
	//impact.rotation = rotation; // TODO:
	impact.rotation = Random::UniformFloat(0.0f, Constants::TwoPi);

	++mGlassData.impactCount;
}