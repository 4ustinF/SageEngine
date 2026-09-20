#pragma once

#include "Component.h"

namespace SAGE
{
	class RenderService;
	class TransformComponent;

	enum DepthMapResolution : uint32_t
	{
		DMPR_256 = 256,
		DMPR_512 = 512,
		DMPR_1024 = 1024,
		DMPR_2048 = 2048,
		DMPR_4096 = 4096
	};

	enum LightMode : uint32_t
	{
		RealTime,
		PseudoBaked,
		Baked,
	};

	class SpotlightComponent final : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::Spotlight)
		MEMORY_POOL_DECLARE

		const char* GetCompName() override { return "Spot Light Component"; }
		void LoadComponentFromTemplate(const rapidjson::Value& value) override;
		void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

		void Initialize() override;
		void Terminate() override;

		void DebugUI() override;

		void OnEnable() override;
		void OnDisable() override;

		//const Math::Vector3& GetPosition();
		//const Math::Vector3& GetDirection();
	/*	float GetInnerConeAngle() const { return mInnerConeAngle; }
		float GetOuterConeAngle() const { return mOuterConeAngle; }
		float GetRange() const { return mRange; }
		const Math::Vector3& GetAttenuation() const { return mAttenuation; }
		float GetAttenuationConstantTerm() const { return mAttenuation.x; }
		float GetAttenuationLinearTerm() const { return mAttenuation.y; }
		float GetAttenuationQuadraticTerm() const { return mAttenuation.z; }
		const Graphics::Color& GetAmbientColor() const { return mAmbientColor; }
		const Graphics::Color& GetDiffuseColor() const { return mDiffuseColor; }
		const Graphics::Color& GetSpecularColor() const { return mSpecularColor; }*/

		void SetPosition(const Math::Vector3& position);
		void SetDirection(const Math::Vector3& direction);
		void SetInnerConeAngle(float innerConeAngle);
		void SetOuterConeAngle(float outerConeAngle);
		void SetRange(float range);

		void SetAttenuation(const Math::Vector3& attenuation);
		void SetAttenuationConstantTerm(float constantTerm);	// Doesn't involve distance at all. It is just added flatly regardless of distance.
		void SetAttenuationLinearTerm(float linearTerm);		// Falloff proportional to distance. A straight, gentle fade.
		void SetAttenuationQuadraticTerm(float quadraticTerm);	// Falloff proportional to distance².

		void SetAmbientColor(const Graphics::Color& color);
		void SetDiffuseColor(const Graphics::Color& color);
		void SetSpecularColor(const Graphics::Color& color);

	private:
		friend class RenderService;
		uint32_t GetDepthMapResolution() const { return static_cast<uint32_t>(mDepthMapResolution); }
		bool GetCanMarkClean() const;
		Graphics::SpotLight& GetSpotLightData() { return mSpotLightData; }
		bool mIsRegisteredWithRenderService = false;

		Graphics::SpotShadowEffect& GetSpotShadowEffect() { return mSpotShadowEffect; }

		void OnTransformPositionChanged(const Math::Vector3& position);
		void OnTransformRotationChanged(const Math::Quaternion& rotation);

		RenderService* mRenderService = nullptr;
		TransformComponent* mTransformComponent = nullptr;

		Core::Delegate::FDelegateHandle OnPositionChangedHandle;
		Core::Delegate::FDelegateHandle OnRotationChangedHandle;

		Graphics::SpotLight mSpotLightData;
		DepthMapResolution mDepthMapResolution = DepthMapResolution::DMPR_1024;
		LightMode mLightMode = LightMode::PseudoBaked;

		const char* LightModeNames[3] = {
			"Real Time",
			"Pseudo Baked",
			"Baked",
		};

		const char* DepthMapResolutionNames[5] = {
			"DMPR_256",
			"DMPR_512",
			"DMPR_1024",
			"DMPR_2048",
			"DMPR_4096",
		};

		const std::vector<DepthMapResolution> DepthMapResolutionValues = {
			DepthMapResolution::DMPR_256,
			DepthMapResolution::DMPR_512,
			DepthMapResolution::DMPR_1024,
			DepthMapResolution::DMPR_2048,
			DepthMapResolution::DMPR_4096,
		};

		Graphics::SpotShadowEffect mSpotShadowEffect;
	};
}