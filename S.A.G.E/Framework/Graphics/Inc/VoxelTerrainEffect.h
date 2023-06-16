#pragma once

#include "BlendState.h"
#include "ConstantBuffer.h"
#include "LightTypes.h"
#include "Material.h"
#include "PixelShader.h"
#include "RenderObject.h"
#include "Sampler.h"
#include "VertexShader.h"

namespace SAGE::Graphics
{
	class Camera;
	class Texture;

	class VoxelTerrainEffect
	{
	public:
		void Initialize(Sampler::Filter sampleFilter = Sampler::Filter::Point);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);
		void SetLightCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetShadowMap(const Texture* shadowMap);
		void SetDepthBias(float bias) { mDepthBias = bias; }
		void SetFogColor(Color fogColor) { mFogColor = fogColor; }
		void SetFogStart(float startDistance) { mFogStart = startDistance; }
		void SetFogEnd(float endDistance) { mFogEnd = endDistance; }
		void SetBlendState(const BlendState::Mode blendStateMode) { mBlendStateMode = blendStateMode; };

		void UseShadowMap(bool use) { mSettingsData.useShadowMap = use ? 1 : 0; }
		void UseFog(bool use) { mSettingsData.useFog = use ? 1 : 0; }

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp[2];
			SAGE::Math::Vector3 viewPosition;
			float padding = 0.0f;
		};

		struct SettingsData
		{
			int useShadowMap = 1;
			float depthBias = 0.0f;
			int useFog = 0;
			float fogStart = 50.0f;

			float fogEnd = 100.0f;
			float padding[3] = {0.0f};

			SAGE::Graphics::Color fogColor = Colors::Gray;
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		using MaterialBuffer = TypedConstantBuffer<Material>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;

		const Camera* mCamera = nullptr;
		const Camera* mLightCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;
		const Texture* mShadowMap = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;
		LightBuffer mLightBuffer;
		MaterialBuffer mMaterialBuffer;
		SettingsBuffer mSettingsBuffer;

		BlendState mAlphaBlendState;
		BlendState::Mode mBlendStateMode = BlendState::Mode::Opaque;

		Sampler mSampler;

		SettingsData mSettingsData;
		float mDepthBias = 0.0f;

		SAGE::Graphics::Color mFogColor = Colors::Gray;
		float mFogStart = 50.0f;
		float mFogEnd = 100.0f;

		const SAGE::Graphics::TextureId mBedrockTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/bedrock.png");
		const SAGE::Graphics::TextureId mGrassTopTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/grass_top.png");
		const SAGE::Graphics::TextureId mGrassSideTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/grass_side.png");
		const SAGE::Graphics::TextureId mDirtTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/dirt.png");
		const SAGE::Graphics::TextureId mCobbleStoneTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/cobblestone.png");
		const SAGE::Graphics::TextureId mStoneTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/stone.png");
		const SAGE::Graphics::TextureId mOakLeafsTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/leafs_oak.png");
		const SAGE::Graphics::TextureId mOakLogTopTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/log_oak_top.png");
		const SAGE::Graphics::TextureId mOakLogSideTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/log_oak.png");
		const SAGE::Graphics::TextureId mOakPlankTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/planks_oak.png");
		const SAGE::Graphics::TextureId mGlassTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/glass.png");
		const SAGE::Graphics::TextureId mSandTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/sand.png");
		const SAGE::Graphics::TextureId mGravelTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/gravel.png");
		const SAGE::Graphics::TextureId mCoalOreTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/coal_ore.png");
		const SAGE::Graphics::TextureId mIronOreTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/iron_ore.png");
		const SAGE::Graphics::TextureId mGoldOreTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/gold_ore.png");
		const SAGE::Graphics::TextureId mDiamondOreTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/diamond_ore.png");
		const SAGE::Graphics::TextureId mRedStoneOreTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/redstone_ore.png");
		const SAGE::Graphics::TextureId mLapisOreTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/lapis_ore.png");
		const SAGE::Graphics::TextureId mEmeraldOreTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/emerald_ore.png");
		const SAGE::Graphics::TextureId mSnowGrassBlockTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/grass_side_snowed.png");
		const SAGE::Graphics::TextureId mSnowTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/snow.png");
		const SAGE::Graphics::TextureId mCactusTopTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/cactus_top.png");
		const SAGE::Graphics::TextureId mCactusSideTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/cactus_side.png");

		const SAGE::Graphics::TextureId mGrassTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/plant_grass.png");
		const SAGE::Graphics::TextureId mPoppyTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/poppy.png");
		const SAGE::Graphics::TextureId mDandelionTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/dandelion.png");
		const SAGE::Graphics::TextureId mSugarCaneTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/reeds.png");

		const SAGE::Graphics::TextureId mCraftingTopTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/crafting_table_top.png");
		const SAGE::Graphics::TextureId mCraftingFrontSideTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/crafting_table_front.png");
		const SAGE::Graphics::TextureId mCraftingSideTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/crafting_table_side.png");
		const SAGE::Graphics::TextureId mFurnaceTopTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/furnace_top.png");
		const SAGE::Graphics::TextureId mFurnaceFrontSideTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/furnace_front_off.png");
		const SAGE::Graphics::TextureId mFurnaceSideTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/furnace_side.png");

		const SAGE::Graphics::TextureId mFarmlandTextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Block/farmland_dry.png");
		const SAGE::Graphics::TextureId mWheat0TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_0.png");
		const SAGE::Graphics::TextureId mWheat1TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_1.png");
		const SAGE::Graphics::TextureId mWheat2TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_2.png");
		const SAGE::Graphics::TextureId mWheat3TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_3.png");
		const SAGE::Graphics::TextureId mWheat4TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_4.png");
		const SAGE::Graphics::TextureId mWheat5TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_5.png");
		const SAGE::Graphics::TextureId mWheat6TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_6.png");
		const SAGE::Graphics::TextureId mWheat7TextureID = SAGE::Graphics::TextureManager::Get()->LoadTexture("../Sprites/SageCraft/Textures/Farm/wheat_stage_7.png");
	};
}