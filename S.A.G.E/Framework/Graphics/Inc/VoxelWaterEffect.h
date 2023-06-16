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

	class VoxelWaterEffect
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
		void SetFogColor(Color fogColor) { mFogColor = fogColor; }
		void SetFogStart(float startDistance) { mFogStart = startDistance; }
		void SetFogEnd(float endDistance) { mFogEnd = endDistance; }
		void SetBlendState(const BlendState::Mode blendStateMode) { mBlendStateMode = blendStateMode; };

		void SetChunkSize(const float size) { mChunkSize = size; }

		void SetDeltaTime(const float deltaTime) { mDeltaTime = deltaTime; }
		void SetAmplitude(const float amplitude) { mAmplitude = amplitude; }
		void SetWaveLength(const float waveLength) { mWaveLength = waveLength; }
		void UseFog(bool use) { mSettingsData.useFog = use ? 1 : 0; }

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp[2];
			SAGE::Math::Vector3 viewPosition;
			float chunkSize = 16.0f;
		};

		struct SettingsData
		{
			float deltaTime = 0.0f;
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float waveLength = 0.1f;

			int useFog = 0;
			float fogStart = 50.0f;
			float fogEnd = 100.0f;
			float padding = 0.0f;

			SAGE::Graphics::Color fogColor = Colors::Gray;
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		using MaterialBuffer = TypedConstantBuffer<Material>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;

		const Camera* mCamera = nullptr;
		const Camera* mLightCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;

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
		float mChunkSize = 16.0f;

		float mDeltaTime = 0.0f;
		float mAmplitude = 0.1f;
		float mFrequency = 0.0f;
		float mWaveLength = 0.1f;

		bool isIncrementing = true;

		SAGE::Graphics::Color mFogColor = Colors::Gray;
		float mFogStart = 50.0f;
		float mFogEnd = 100.0f;
	};
}