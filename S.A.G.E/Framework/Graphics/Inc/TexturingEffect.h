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
		void Initialize();
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
		using TransformBuffer = TypedConstantBuffer<Math::Matrix4>;

		const Camera* mCamera = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;

		Sampler mSampler;
	};
}