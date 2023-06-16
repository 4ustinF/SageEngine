#pragma once

#include "BlendState.h"
#include "ConstantBuffer.h"
#include "LightTypes.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "TextureManager.h"
#include "VertexShader.h"

namespace SAGE::Graphics
{
	class Camera;
	class RenderObject;

	class ToonEffect
	{
	public:
		void Initialize();
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetCelMap(const TextureId* celMap);

		void UseDiffuseMap(bool use) { mSettingsData.useDiffuseMap = use ? 1 : 0; };
		void UseCelMap(bool use) { mSettingsData.useCelMap = use ? 1 : 0; };
		void UseRim(bool use) { mSettingsData.useRim = use ? 1 : 0; };

		void SetRimIntensity(float intensity);

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp;
			SAGE::Math::Vector3 viewPosition;
			float bumpWeight = 0.0f;
		};

		struct SettingsData
		{
			int useDiffuseMap = 1;
			int useCelMap = 0;
			int useRim = 1;
			float rimIntensity = 0.7f;
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;

		const Camera* mCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;
		LightBuffer mLightBuffer;
		SettingsBuffer mSettingsBuffer;

		Sampler mSampler;

		SettingsData mSettingsData;

		float mRimIntensity = 0.7f;
		const TextureId* mCelmap;
	};
}