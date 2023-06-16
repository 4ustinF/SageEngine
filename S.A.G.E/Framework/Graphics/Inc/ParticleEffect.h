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

	class ParticleEffect
	{
	public:
		void Initialize(Sampler::Filter sampleFilter = Sampler::Filter::Linear);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);
		void SetBlendState(const BlendState::Mode blendStateMode) { mBlendStateMode = blendStateMode; };
		void SetParticleColor(SAGE::Graphics::Color particleColor) { mParticleColor = particleColor; }
		void SetUVOffSet(const SAGE::Math::Vector2 uvOffSet) { mUVOffSet = uvOffSet; }

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp;
			SAGE::Math::Vector3 viewPosition;
			float padding = 0.0f;
		};

		struct SettingsData
		{
			SAGE::Graphics::Color particleColor = SAGE::Graphics::Colors::White;
			SAGE::Math::Vector2 uvOffSet = { 0.0f, 0.0f };
			float padding[2] = {};
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;

		const Camera* mCamera = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;
		SettingsBuffer mSettingsBuffer;

		BlendState mAlphaBlendState;
		BlendState::Mode mBlendStateMode = BlendState::Mode::Opaque;

		Sampler mSampler;
		SettingsData mSettingsData;

		SAGE::Graphics::Color mParticleColor = SAGE::Graphics::Colors::White;
		SAGE::Math::Vector2 mUVOffSet = { 0.0f, 0.0f };
	};
}