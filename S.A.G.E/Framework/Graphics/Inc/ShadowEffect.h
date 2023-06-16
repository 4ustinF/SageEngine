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

	class ShadowEffect
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

		void SetDirectionalLight(const DirectionalLight& directionalLight);

		void SetFocus(const Math::Vector3& focusPosition) { mFocusPosition = focusPosition; }
		void SetSize(float size) { mSize = size; }

		const Camera& GetLightCamera() const { return mLightCamera; }
		const Texture& GetDepthMap() const { return mDepthMapRenderTarget; }

		void DebugUI();

	private:
		void UpdateLightCamera();

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

		const DirectionalLight* mDirectionalLight = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;
		BoneTransformBuffer mBoneTransformBuffer;
		SettingsBuffer mSettingsBuffer;

		RenderTarget mDepthMapRenderTarget;

		Math::Vector3 mFocusPosition = Math::Vector3::Zero;
		float mSize = 100.0f;
	};
}