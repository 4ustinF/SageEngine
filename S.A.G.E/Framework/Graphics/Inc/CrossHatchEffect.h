#pragma once

#include "ConstantBuffer.h"
#include "LightTypes.h"
#include "Material.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "TextureManager.h"
#include "VertexShader.h"
#include "RenderObject.h"

namespace SAGE::Graphics
{
	class Camera;
	class RenderObject;

	class CrossHatchEffect
	{
	public:
		void Initialize();
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderGroup& renderGroup);
		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetIntensityMultiplier(float multiplier) { mIntensity = multiplier; }
		void SetHatchMapDark(std::filesystem::path fileName) { mHatchMapDark = TextureManager::Get()->LoadTexture(fileName); }
		void SetHatchMapBright(std::filesystem::path fileName) { mHatchMapBright = TextureManager::Get()->LoadTexture(fileName); }

		void UseDiffuseMap(bool use) { mSettingsData.useDiffuseMap = use ? 1 : 0; };
		void UseSpecularMap(bool use) { mSettingsData.useSpecularMap = use ? 1 : 0; };
		void UseNormalMap(bool use) { mSettingsData.useNormalMap = use ? 1 : 0; };

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp[2];
			SAGE::Math::Vector3 viewPosition;
			float bumpWeight = 0.0f;
		};

		struct SettingsData
		{
			int useDiffuseMap = 1;
			int useSpecularMap = 1;
			int useNormalMap = 1;
			float param0 = 1.0f; // Intensity multiplier 
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using LightBuffer = TypedConstantBuffer<DirectionalLight>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;
		using MaterialBuffer = TypedConstantBuffer<Material>;

		const Camera* mCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;

		VertexShader mVertexShader;
		PixelShader mPixelShader;

		TransformBuffer mTransformBuffer;
		LightBuffer mLightBuffer;
		SettingsBuffer mSettingsBuffer;
		MaterialBuffer mMaterialBuffer;

		Sampler mSampler;

		SettingsData mSettingsData;

		TextureId mHatchMapDark = 0;
		TextureId mHatchMapBright = 0;
		float mIntensity = 1.0f;
	};
}