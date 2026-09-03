#include "Precompiled.h"
#include "StandardEffect.h"

#include "AnimationUtil.h"
#include "Animator.h"
#include "Camera.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void StandardEffect::Initialize(Sampler::Filter sampleFilter)
{
	mVertexShader.Initialize<Vertex>(L"../../Assets/Shaders/Standard.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/Standard.fx");

	mSpotLightBuffer.Initialize();
	mSpotShadowMatrixBuffer.Initialize();
	mTransformBuffer.Initialize();
	mBoneTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mSettingsBuffer.Initialize();

	mAlphaBlendState.Initialize(mBlendStateMode);
	mSampler.Initialize(sampleFilter, Sampler::AddressMode::Wrap);
}

void StandardEffect::Terminate()
{
	mSampler.Terminate();

	mAlphaBlendState.Terminate();

	mSpotLightBuffer.Terminate();
	mSpotShadowMatrixBuffer.Terminate();
	mSettingsBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mLightBuffer.Terminate();
	mBoneTransformBuffer.Terminate();
	mTransformBuffer.Terminate();

	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void StandardEffect::Begin()
{
	ASSERT(mCamera != nullptr, "StandardEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "StandardEffect - No light set");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mTransformBuffer.BindVS(0);

	mBoneTransformBuffer.BindVS(1);

	mLightBuffer.BindVS(2);
	mLightBuffer.BindPS(2);

	mMaterialBuffer.BindPS(3);

	mSettingsBuffer.BindVS(4);
	mSettingsBuffer.BindPS(4);

	mSpotLightBuffer.BindPS(5);
	mSpotShadowMatrixBuffer.BindPS(6);

	mSampler.BindVS(0);
	mSampler.BindPS(0);
}

void StandardEffect::End()
{
	if (mShadowMap != nullptr) {
		Texture::UnbindPS(4);
	}

	for (size_t i = 0; i < mActiveSpotLightCount; ++i) {
		Texture::UnbindPS(5 + static_cast<uint32_t>(i));
	}
}

void StandardEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) {
		Render(renderObjects);
	}
}

void StandardEffect::Render(const RenderObject& renderObject)
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

	if (renderObject.animator)
	{
		BoneTransformData boneTransformData;

		std::vector<Math::Matrix4> boneTransforms;
		AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [animator = renderObject.animator](const Bone* bone) {return animator->GetTransform(bone); });
		AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

		const size_t boneCount = renderObject.skeleton->bones.size();
		for (size_t i = 0; i < boneCount && i < BoneTransformData::MaxBoneCount; ++i) {
			boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
		}

		mBoneTransformBuffer.Update(boneTransformData);
		settingsData.useSkinning = 1;
	}
	else if (renderObject.skeleton)
	{
		BoneTransformData boneTransformData;

		std::vector<Math::Matrix4> boneTransforms;
		AnimationUtil::ComputeBoneTransforms(*renderObject.skeleton, boneTransforms, [](const Bone* bone) {return bone->toParentTransform; });
		AnimationUtil::ApplyBoneOffset(*renderObject.skeleton, boneTransforms);

		const size_t boneCount = renderObject.skeleton->bones.size();
		for (size_t i = 0; i < boneCount && i < BoneTransformData::MaxBoneCount; ++i) {
			boneTransformData.boneTransforms[i] = Math::Transpose(boneTransforms[i]);
		}

		mBoneTransformBuffer.Update(boneTransformData);
		settingsData.useSkinning = 1;
	}

	settingsData.tiling = renderObject.tilingSize;
	settingsData.tilingOffset = renderObject.tilingOffset;

	mSettingsBuffer.Update(settingsData);

	mSpotLightBuffer.Update(mSpotLightBufferData);
	mSpotShadowMatrixBuffer.Update(mSpotShadowMatrixData);
	for (size_t i = 0; i < mActiveSpotLightCount; ++i)
	{
		if (mSpotShadowMaps[i])
		{
			mSpotShadowMaps[i]->BindPS(5 + static_cast<uint32_t>(i));
		}
	}
	settingsData.useSpotShadows = mSettingsData.useSpotShadows;

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specularMapId, 1);
	tm->BindVS(renderObject.bumpMapId, 2);
	tm->BindPS(renderObject.normalMapId, 3);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void StandardEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void StandardEffect::SetLightCamera(const Camera& camera)
{
	mLightCamera = &camera;
}

void StandardEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void StandardEffect::SetShadowMap(const Texture* shadowMap)
{
	mShadowMap = shadowMap;
}

void StandardEffect::SetSpotLights(const SpotLight* lights, size_t count)
{
	ASSERT(lights != nullptr || count == 0, "StandardEffect -- lights cannot be null when count > 0");

	mActiveSpotLightCount = std::min(count, MaxSpotLights);
	for (size_t i = 0; i < mActiveSpotLightCount; ++i) {
		mSpotLightBufferData.spotLights[i] = lights[i];
	}
	mSpotLightBufferData.spotLightCount = static_cast<int>(mActiveSpotLightCount);
}

void StandardEffect::SetSpotShadowMap(size_t index, const Texture* shadowMap)
{
	ASSERT(index < MaxSpotLights, "StandardEffect -- spot shadow map index out of range");
	mSpotShadowMaps[index] = shadowMap;
}

void StandardEffect::SetSpotLightViewProj(size_t index, const Math::Matrix4& viewProj)
{
	ASSERT(index < MaxSpotLights, "StandardEffect -- spot light view proj index out of range");
	mSpotShadowMatrixData.viewProj[index] = Math::Transpose(viewProj);
}

void StandardEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Standard Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Bump Weight##StandardEffect", &mBumpWeight, 0.01f, 0.0f, 5.0f);

		bool useDiffuseMap = (mSettingsData.useDiffuseMap == 1);
		ImGui::Checkbox("Use Diffuse Map##StandardEffect", &useDiffuseMap);
		mSettingsData.useDiffuseMap = useDiffuseMap ? 1 : 0;

		bool useSpecularMap = (mSettingsData.useSpecularMap == 1);
		ImGui::Checkbox("Use Specular Map##StandardEffect", &useSpecularMap);
		mSettingsData.useSpecularMap = useSpecularMap ? 1 : 0;

		bool useBumpMap = (mSettingsData.useBumpMap == 1);
		ImGui::Checkbox("Use Bump Map##StandardEffect", &useBumpMap);
		mSettingsData.useBumpMap = useBumpMap ? 1 : 0;

		bool useNormalMap = (mSettingsData.useNormalMap == 1);
		ImGui::Checkbox("Use Normal Map##StandardEffect", &useNormalMap);
		mSettingsData.useNormalMap = useNormalMap ? 1 : 0;

		bool useShadowMap = (mSettingsData.useShadowMap == 1);
		ImGui::Checkbox("Use Shadow Map", &useShadowMap);
		mSettingsData.useShadowMap = useShadowMap ? 1 : 0;

		bool useFog = (mSettingsData.useFog == 1);
		ImGui::Checkbox("Use Fog##StandardEffect", &useFog);
		mSettingsData.useFog = useFog ? 1 : 0;

		ImGui::DragFloat("Fog Start##StandardEffect", &mFogStart, 1.0f, 0.0f, 250.0f);
		ImGui::DragFloat("Fog End##StandardEffect", &mFogEnd, 1.0f, 0.0f, 250.0f);
		ImGui::ColorEdit4("Fog Color##FogEffect", &mFogColor.r);

		//bool useSkinning = (mSettingsData.useSkinning == 1);
		//ImGui::Checkbox("Use Skinning", &useSkinning);
		//mSettingsData.useSkinning = useSkinning ? 1 : 0;

		ImGui::DragFloat("Depth Bias##StandardEffect", &mDepthBias, 0.000001f, 0.0f, 1.0f, "%.6f");
		ImGui::DragInt("Sample Size##StandardEffect", &mSampleSize, 0.1f, 0, 5);
	}
}