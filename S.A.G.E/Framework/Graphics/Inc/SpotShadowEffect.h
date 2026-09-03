#pragma once

#include "Camera.h"
#include "ConstantBuffer.h"
#include "LightTypes.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "RenderObject.h"
#include "VertexShader.h"

namespace SAGE::Graphics
{
	class RenderObject;

	class SpotShadowEffect
	{
	public:
		void Initialize(uint32_t resolution = 1024);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);

		void SetSpotLight(const SpotLight& spotLight);

		const Camera& GetLightCamera() const { return mLightCamera; }
		const Texture& GetDepthMap() const { return mDepthMapRenderTarget; }

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 wvp;
		};

		struct BoneTransformData
		{
			static constexpr size_t MaxBoneCount = 128;
			Math::Matrix4 boneTransforms[MaxBoneCount];
		};

		struct SettingsData
		{
			int useSkinning = 0;
			float padding[3] = {};
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using BoneTransformBuffer = TypedConstantBuffer<BoneTransformData>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;

		Camera mLightCamera;
		const SpotLight* mSpotLight = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;
		BoneTransformBuffer mBoneTransformBuffer;
		SettingsBuffer mSettingsBuffer;

		RenderTarget mDepthMapRenderTarget;
	};
}