#include "Precompiled.h"
#include "VoxelTerrainEffect.h"

#include "Camera.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void VoxelTerrainEffect::Initialize(Sampler::Filter sampleFilter)
{
	mVertexShader.Initialize<VoxelVertex>(L"../../Assets/Shaders/VoxelTerrain.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/VoxelTerrain.fx");

	mTransformBuffer.Initialize();
	mLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mSettingsBuffer.Initialize();

	mAlphaBlendState.Initialize(mBlendStateMode);
	mSampler.Initialize(sampleFilter, Sampler::AddressMode::Wrap);
}

void VoxelTerrainEffect::Terminate()
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

void VoxelTerrainEffect::Begin()
{
	ASSERT(mCamera != nullptr, "VoxelTerrainEffect - No camera set");
	ASSERT(mDirectionalLight != nullptr, "VoxelTerrainEffect - No light set");
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

void VoxelTerrainEffect::End()
{
	if (mShadowMap != nullptr) {
		Texture::UnbindPS(4);
	}
}

void VoxelTerrainEffect::Render(const RenderGroup& renderGroup)
{
	for (auto& renderObjects : renderGroup) {
		Render(renderObjects);
	}
}

void VoxelTerrainEffect::Render(const RenderObject& renderObject)
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
	}
	else {
		settingsData.useShadowMap = 0;
	}

	mSettingsBuffer.Update(settingsData);

	auto tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specularMapId, 1);
	tm->BindVS(renderObject.bumpMapId, 2);
	tm->BindPS(renderObject.normalMapId, 3);

	tm->BindPS(mBedrockTextureID, 5);
	tm->BindPS(mGrassTopTextureID, 6);
	tm->BindPS(mGrassSideTextureID, 7);
	tm->BindPS(mDirtTextureID, 8);
	tm->BindPS(mCobbleStoneTextureID, 9);
	tm->BindPS(mStoneTextureID, 10);
	tm->BindPS(mOakLeafsTextureID, 11);
	tm->BindPS(mOakLogTopTextureID, 12);
	tm->BindPS(mOakLogSideTextureID, 13);
	tm->BindPS(mOakPlankTextureID, 14);
	tm->BindPS(mGlassTextureID, 15);
	tm->BindPS(mSandTextureID, 16);
	tm->BindPS(mGravelTextureID, 17);
	tm->BindPS(mCoalOreTextureID, 18);
	tm->BindPS(mIronOreTextureID, 19);
	tm->BindPS(mGoldOreTextureID, 20);
	tm->BindPS(mDiamondOreTextureID, 21);
	tm->BindPS(mRedStoneOreTextureID, 22);
	tm->BindPS(mLapisOreTextureID, 23);
	tm->BindPS(mEmeraldOreTextureID, 24);
	tm->BindPS(mSnowGrassBlockTextureID, 25);
	tm->BindPS(mSnowTextureID, 26);
	tm->BindPS(mCactusTopTextureID, 27);
	tm->BindPS(mCactusSideTextureID, 28);

	tm->BindPS(mGrassTextureID, 29);
	tm->BindPS(mPoppyTextureID, 30);
	tm->BindPS(mDandelionTextureID, 31);
	tm->BindPS(mSugarCaneTextureID, 32);

	tm->BindPS(mCraftingTopTextureID, 33);
	tm->BindPS(mCraftingFrontSideTextureID, 34);
	tm->BindPS(mCraftingSideTextureID, 35);
	tm->BindPS(mFurnaceTopTextureID, 36);
	tm->BindPS(mFurnaceFrontSideTextureID, 37);
	tm->BindPS(mFurnaceSideTextureID, 38);

	tm->BindPS(mFarmlandTextureID, 39);
	tm->BindPS(mWheat0TextureID, 40);
	tm->BindPS(mWheat1TextureID, 41);
	tm->BindPS(mWheat2TextureID, 42);
	tm->BindPS(mWheat3TextureID, 43);
	tm->BindPS(mWheat4TextureID, 44);
	tm->BindPS(mWheat5TextureID, 45);
	tm->BindPS(mWheat6TextureID, 46);
	tm->BindPS(mWheat7TextureID, 47);

	mAlphaBlendState.Set();
	renderObject.meshBuffer.Render();
	BlendState::ClearState();
}

void VoxelTerrainEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void VoxelTerrainEffect::SetLightCamera(const Camera& camera)
{
	mLightCamera = &camera;
}

void VoxelTerrainEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void VoxelTerrainEffect::SetShadowMap(const Texture* shadowMap)
{
	mShadowMap = shadowMap;
}

void VoxelTerrainEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("Voxel Terrain Effect", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		bool useShadowMap = (mSettingsData.useShadowMap == 1);
		ImGui::Checkbox("Use Shadow Map##VoxelTerrainEffect", &useShadowMap);
		mSettingsData.useShadowMap = useShadowMap ? 1 : 0;

		bool useFog = (mSettingsData.useFog == 1);
		ImGui::Checkbox("Use Fog##VoxelTerrainEffect", &useFog);
		mSettingsData.useFog = useFog ? 1 : 0;

		ImGui::DragFloat("Fog Start##VoxelTerrainEffect", &mFogStart, 1.0f, 0.0f, 250.0f);
		ImGui::DragFloat("Fog End##VoxelTerrainEffect", &mFogEnd, 1.0f, 0.0f, 250.0f);
		ImGui::ColorEdit4("Fog Color##VoxelTerrainEffect", &mFogColor.r);

		ImGui::DragFloat("Depth Bias##VoxelTerrainEffect", &mDepthBias, 0.000001f, 0.0f, 1.0f, "%.6f");
	}
}