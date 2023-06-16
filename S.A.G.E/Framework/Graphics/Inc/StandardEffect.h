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

	class StandardEffect
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

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp[2];
			SAGE::Math::Vector3 viewPosition;
			float bumpWeight = 0.0f;
		};

		struct BoneTransformData
		{
			static constexpr size_t MaxBoneCount = 128;
			Math::Matrix4 boneTransforms[MaxBoneCount];
		};

		struct SettingsData
		{
			int useDiffuseMap = 1;
			int useSpecularMap = 1;
			int useBumpMap = 1;
			int useNormalMap = 1;

			int useShadowMap = 1;
			int useSkinning = 0;
			float depthBias = 0.0f;
			int sampleSize = 1;

			SAGE::Graphics::Color fogColor = Colors::Gray;
			int useFog = 0;
			float fogStart = 50.0f;
			float fogEnd = 100.0f;
			float padding = 0;
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using BoneTransformBuffer = TypedConstantBuffer<BoneTransformData>;
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
		BoneTransformBuffer mBoneTransformBuffer;
		LightBuffer mLightBuffer;
		MaterialBuffer mMaterialBuffer;
		SettingsBuffer mSettingsBuffer;

		BlendState mAlphaBlendState;
		BlendState::Mode mBlendStateMode = BlendState::Mode::Opaque;

		Sampler mSampler;

		SettingsData mSettingsData;
		float mBumpWeight = 0.0f;
		float mDepthBias = 0.0f;
		int mSampleSize = 1;

		SAGE::Graphics::Color mFogColor = Colors::Gray;
		float mFogStart = 50.0f;
		float mFogEnd = 100.0f;
	};
}