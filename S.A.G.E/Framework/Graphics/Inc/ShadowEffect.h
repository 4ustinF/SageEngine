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
		void Initialize(uint32_t depthMapResolution = 4096);
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);

		void DebugUI();

		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetFocus(const Math::Vector3& focusPosition);
		void SetSize(float size);

		const Camera& GetLightCamera() const { return mLightCamera; }
		const Texture& GetDepthMap() const { return mDepthMapRenderTarget; }

		bool NeedsUpdate() const;
		void MarkClean();
		void Invalidate();

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

		// ---------------------------------------- Temp baking ----------------------------------------
		bool mIsDirty = true; // Starts true so the first frame always renders
		Math::Vector3 mBakedDirection = Math::Vector3::Zero;

		Math::Vector3 mFocusPosition = Math::Vector3::Zero;
		float mSize = 100.0f;
	};
}