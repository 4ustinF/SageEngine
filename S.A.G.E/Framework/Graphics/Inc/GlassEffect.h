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

	class GlassEffect
	{
	public:
		void Initialize(Sampler::Filter sampleFilter = Sampler::Filter::Linear);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);
		void SetLightCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetBumpWeight(float weight) { mBumpWeight = weight; }
		void SetShadowMap(const Texture* shadowMap);
		void SetDepthBias(float bias) { mDepthBias = bias; }
		void SetSampleSize(int size) { mSampleSize = size; }
		void SetFogColor(Color fogColor) { mFogColor = fogColor; }
		void SetFogStart(float startDistance) { mFogStart = startDistance; }
		void SetFogEnd(float endDistance) { mFogEnd = endDistance; }
		void SetBlendState(const BlendState::Mode blendStateMode) { mBlendStateMode = blendStateMode; };

		void UseDiffuseMap(bool use) { mSettingsData.useDiffuseMap = use ? 1 : 0; };
		void UseSpecularMap(bool use) { mSettingsData.useSpecularMap = use ? 1 : 0; };
		void UseBumpMap(bool use) { mSettingsData.useBumpMap = use ? 1 : 0; };
		void UseNormalMap(bool use) { mSettingsData.useNormalMap = use ? 1 : 0; };
		void UseShadowMap(bool use) { mSettingsData.useShadowMap = use ? 1 : 0; }
		void UseFog(bool use) { mSettingsData.useFog = use ? 1 : 0; }

		void UseShatterNormalMap(bool use) { mGlassData.useShatterNormalMap = use ? 1 : 0; }

		void SetShatterIntensity(float intensity) { mGlassData.shatterIntensity = intensity; }

		void SetShatterMapId(Graphics::TextureId textureId) { mShatterMapId = textureId; }
		void SetShatterNormalMapId(Graphics::TextureId textureId) { mShatterNormalMapId = textureId; }

		void ClearImpacts();
		void AddImpact(const Math::Vector2& uv, float radius, float strength, float rotation);

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp[2];
			SAGE::Math::Vector3 viewPosition;
			float bumpWeight = 0.0f;
		};

		struct SettingsData // TODO: Fix padding issue.
		{
			int useDiffuseMap = 1;
			int useSpecularMap = 1;
			int useBumpMap = 1;
			int useNormalMap = 1;

			int useShadowMap = 1;
			float depthBias = 0.0f;
			int sampleSize = 1;
			float padding0 = 0.0f;

			SAGE::Graphics::Color fogColor = Colors::Gray;
			int useFog = 0;
			float fogStart = 50.0f;
			float fogEnd = 100.0f;
			float padding1 = 0;

			SAGE::Math::Vector2 tiling = { 1.0f, 1.0f };
			SAGE::Math::Vector2 tilingOffset = { 0.0f, 0.0f };
		};

		static constexpr int MaxGlassImpacts = 3;

		struct GlassImpactData
		{
			SAGE::Math::Vector2 uv = { 0.0f, 0.0f };
			float radius = 0.0f;
			float strength = 0.0f;

			float rotation;
			SAGE::Math::Vector3 padding;
		};

		struct GlassData
		{
			int useShatterNormalMap = 1;
			float shatterIntensity = 1.0f;
			int impactCount = 0;
			float glassPadding = 0.0f;

			GlassImpactData impacts[MaxGlassImpacts];
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		using MaterialBuffer = TypedConstantBuffer<Material>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;
		using GlassBuffer = TypedConstantBuffer<GlassData>;

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
		BlendState::Mode mBlendStateMode = BlendState::Mode::AlphaBlend;

		Sampler mSampler;

		SettingsData mSettingsData;
		float mBumpWeight = 0.0f;
		float mDepthBias = 0.0f;
		int mSampleSize = 1;

		Graphics::Color mFogColor = Colors::Gray;
		float mFogStart = 50.0f;
		float mFogEnd = 100.0f;

		GlassBuffer mGlassBuffer;
		GlassData mGlassData;

		Graphics::TextureId mShatterMapId = 0;
		Graphics::TextureId mShatterNormalMapId = 0;
	};
}