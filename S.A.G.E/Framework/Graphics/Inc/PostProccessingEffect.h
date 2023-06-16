#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "VertexShader.h"

namespace SAGE::Graphics
{
	class RenderObject;
	class Texture;

	class PostProcessingEffect
	{
	public:
		enum class Mode
		{
			None,
			Monochrome,
			Invert,
			Mirror,
			Grain,
			ObjectiveGrain,
			Vignette,
			Sinethresholder,
			Blur,
			Blur2,
			Combine2,
			NightVision
		};

		void Initialize();
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);

		void SetTexture(const Texture* texture, uint32_t slot = 0);
		void SetMode(Mode mode);

		void SetIntensity(float intensity) { mIntensity = intensity; }
		void SetVignetteExtend(float extend) { mVignetteExtend = extend; }
		void SetSineAmount(float amount) { mSineAmount = amount; }

		void DebugUI();

	private:
		struct PostProcessData
		{
			Mode mode = Mode::None;
			float param0 = 0.0f;
			float param1 = 0.0f;
			float param2 = 0.0f;
		};

		using PostProcessBuffer = TypedConstantBuffer<PostProcessData>;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		PostProcessBuffer mPostProcessBuffer;

		Sampler mSampler;

		std::array<const Texture*, 4> mTextures;

		Mode mMode = Mode::None;
		float mIntensity = 10.0f;
		float mVignetteExtend = 10.0f;
		float mSineAmount = 0.0f;
		float blurBrightness = 0.6f;
		int blurAmount = 3;
		float blurOffset = 0.002f;
		float mGlowDistance = 3.1f;
		float mGlowSpeed = 4.0f;
	};
}