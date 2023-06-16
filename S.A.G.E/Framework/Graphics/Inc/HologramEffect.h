#pragma once

#include "BlendState.h"
#include "ConstantBuffer.h"
#include "LightTypes.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "VertexShader.h"

namespace SAGE::Graphics
{
	class Camera;
	class RenderObject;

	class HologramEffect
	{
	public:
		void Initialize();
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);

		void SetCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);

		// Settings Data
		void UseDiffuseMap(bool use) { mSettingsData.useDiffuseMap = use ? 1 : 0; }
		void UseBars(bool use) { mSettingsData.useBars = use ? 1 : 0; }
		void UseAlpha(bool use) { mSettingsData.useAlpha = use ? 1 : 0; }
		void ShouldFlicker(bool use) { mSettingsData.shouldFlicker = use ? 1 : 0; }
		void HasRim(bool use) { mSettingsData.hasRim = use ? 1 : 0; }
		void HasGlow(bool use) { mSettingsData.hasGlow = use ? 1 : 0; }

		// Hologram Data
		float GetBarSpeed() { return mBarSpeed; }
		float GetBarDistance() { return mBarDistance; }
		float GetAlpha() { return mAlpha; }
		float GetFlickerSpeed() { return mFlickerSpeed; }
		float GetRimPower() { return mRimPower; }
		float GetGlowDistance() { return mGlowDistance; }
		float GetGlowSpeed() { return mGlowSpeed; }
		Color GetRimColor() { return mRimColor; }
		Color GetMainColor() { return mMainColor; }

		void SetBarSpeed(float speed) { mBarSpeed = speed; }
		void SetBarDistance(float distance) { mBarDistance = distance; }
		void SetAlpha(float alpha) { mAlpha = alpha; } // This is currently clamped between 0.65f and 1.0f in the shader
		void SetFlickerSpeed(float speed) { mFlickerSpeed = speed; }
		void SetRimPower(float power) { mRimPower = power; }
		void SetGlowDistance(float distance) { mGlowDistance = distance; }
		void SetGlowSpeed(float speed) { mGlowSpeed = speed; }
		void SetRimColor(Color color) { mRimColor = color; }
		void SetMainColor(Color color) { mMainColor = color; }

		void DebugUI();

	private:
		struct TransformData
		{
			SAGE::Math::Matrix4 world;
			SAGE::Math::Matrix4 wvp;
			SAGE::Math::Vector3 viewPosition;
			float padding = 0.0f;
		};

		struct SettingsData
		{
			int useDiffuseMap = 1;
			int useBars = 1;
			int useAlpha = 1;
			int shouldFlicker = 1;
			int hasRim = 1;
			int hasGlow = 1;
			int shouldGlitch = 1;
			int bParam0 = 1;
		};

		struct HologramData
		{
			float deltaTime = 1.0f;
			float barSpeed = 1.0f;
			float barDistance = 1.0f;
			float mAlpha = 1.0f;
			float flickerSpeed = 1.0f;
			float rimPower = 1.0f;
			float glowDistance = 1.0f;
			float glowSpeed = 1.0f;
			Color rimColor = Colors::Blue; // Color to float4
			Color mainColor = Colors::White; // Color to float4
			float glitchIntensity = 1.0f;
			float glitchSpeed = 5.0f;
			float param0 = 1.0f;
			float param1 = 1.0f;
		};

		using TransformBuffer = TypedConstantBuffer<TransformData>;
		using SettingsBuffer = TypedConstantBuffer<SettingsData>;
		using HologramBuffer = TypedConstantBuffer<HologramData>;

		TransformBuffer mTransformBuffer;
		SettingsBuffer mSettingsBuffer;
		HologramBuffer mHologramBuffer;

		const Camera* mCamera = nullptr;
		const DirectionalLight* mDirectionalLight = nullptr;

		SAGE::Graphics::VertexShader mVertexShader;
		SAGE::Graphics::PixelShader mPixelShader;

		Sampler mSampler;

		SettingsData mSettingsData;
		float mPadding = 0.0f;

		BlendState mAlphaBlendState;

		float mBarSpeed = 1.0f;
		float mBarDistance = 1.0f;
		float mAlpha = 1.0f;
		float mFlickerSpeed = 1.0f;
		float mRimPower = 1.0f;
		float mGlowDistance = 3.1f;
		float mGlowSpeed = 4.0f;
		Color mRimColor = Colors::Blue;
		Color mMainColor = Colors::White;

		float mGlitchIntensity = 1.0f;
		float mGlitchSpeed = 5.0f;
	};
}