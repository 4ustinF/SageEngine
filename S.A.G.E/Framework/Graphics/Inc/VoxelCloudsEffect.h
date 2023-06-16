#pragma once

#include "BlendState.h"
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

	class VoxelCloudsEffect
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

		SAGE::Math::Vector2& GetUVs() { return mUVs; }
		void SetUVs(const SAGE::Math::Vector2 uv) { mUVs = uv; }

		void SetCloudAlpha(float alpha) { mCloudAlpha = Math::Clamp(alpha, 0.0f, 1.0f); }
		void SetCloudFilter(const Sampler::Filter& filter);

		//void UseFast(bool use) { mSettingsBuffer.useFast = use ? 1 : 0; }
		void UseFast(bool use) { mUseFast = use; }

	private:
		using TransformBuffer = TypedConstantBuffer<Math::Matrix4>;
		TransformBuffer mTransformBuffer;

		struct SettingsData
		{
			int useFast = 1;
			float padding[3] = {};
		};

		struct SkyData
		{
			SAGE::Math::Vector2 translation = SAGE::Math::Vector2::One;
			float scale = 1.0f;
			float brightness = 1.0f;
		};

		using SettingsBuffer = TypedConstantBuffer<SettingsData>;
		SettingsBuffer mSettingsBuffer;

		using SkyBuffer = TypedConstantBuffer<SkyData>;
		SkyBuffer mSkyBuffer;

		const Camera* mCamera = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		Sampler mSampler;
		BlendState mAlphaBlendState;

		SAGE::Math::Vector2 mUVs;
		float mCloudAlpha = 1.0f;
		bool mUseFast = true;
	};
}