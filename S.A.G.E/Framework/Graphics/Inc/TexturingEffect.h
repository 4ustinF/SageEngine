#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "RenderObject.h"
#include "Sampler.h"
#include "VertexShader.h"

namespace SAGE::Graphics
{
	class Camera;
	class RenderObject;
	class Texture;

	class TexturingEffect
	{
	public:
		void Initialize(Sampler::Filter filter = Sampler::Filter::Linear, Sampler::AddressMode addressMode = Sampler::AddressMode::Wrap);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup)
		{
			for (auto& renderObjects : renderGroup) {
				Render(renderObjects);
			}
		}
		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);

		void DebugUI();

	private:
		struct ConstantData
		{
			SAGE::Math::Matrix4 wvp;
			SAGE::Math::Vector2 tiling;
			SAGE::Math::Vector2 other;
		};
		using TransformBuffer = TypedConstantBuffer<ConstantData>;

		const Camera* mCamera = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;

		Sampler mSampler;
	};
}