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

	class ReflectionEffect
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
		void SetBlendState(const BlendState::Mode blendStateMode) { mBlendStateMode = blendStateMode; };

		void SetSourceTexture(const Texture& texture) { mSourceTexture = &texture; }

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp[2];
			SAGE::Math::Vector3 viewPosition;
			float padding = 0.0f;
		};

		struct ReflectionData
		{
			SAGE::Math::Matrix4 reflectionMatrix;
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		using MaterialBuffer = TypedConstantBuffer<Material>;
		using ReflectionBuffer = TypedConstantBuffer<ReflectionData>;

		const Camera* mCamera = nullptr;
		const Camera* mLightCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;
		LightBuffer mLightBuffer;
		MaterialBuffer mMaterialBuffer;
		ReflectionBuffer mReflectionBuffer;

		BlendState mAlphaBlendState;
		BlendState::Mode mBlendStateMode = BlendState::Mode::Opaque;

		Sampler mSampler;

		ReflectionData mReflectionData;
		const Texture* mSourceTexture = nullptr;
	};
}