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

		const Math::Vector3& GetPosition() { return mSpotLightData.position; }
		const Math::Vector3& GetDirection() { return mSpotLightData.direction; }
		float GetInnerConeAngle() const { return mSpotLightData.innerConeAngle * Math::Constants::RadToDeg; }
		float GetOuterConeAngle() const { return mSpotLightData.outerConeAngle * Math::Constants::RadToDeg; }
		float GetRange() const { return mSpotLightData.range; }
		const Math::Vector3& GetAttenuation() const { return mSpotLightData.attenuation; }
		float GetAttenuationConstantTerm() const { return mSpotLightData.attenuation.x; }
		float GetAttenuationLinearTerm() const { return mSpotLightData.attenuation.y; }
		float GetAttenuationQuadraticTerm() const { return mSpotLightData.attenuation.z; }
		const Graphics::Color& GetAmbientColor() const { return mSpotLightData.ambient; }
		const Graphics::Color& GetDiffuseColor() const { return mSpotLightData.diffuse; }
		const Graphics::Color& GetSpecularColor() const { return mSpotLightData.specular; }

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
		Graphics::SpotShadowEffect& GetSpotShadowEffect() { return mSpotShadowEffect; }
		Graphics::SpotLight& GetSpotLightData() { return mSpotLightData; }

		void OnTransformPositionChanged(const Math::Vector3& position);
		void OnTransformRotationChanged(const Math::Quaternion& rotation);

		RenderService* mRenderService = nullptr;
		TransformComponent* mTransformComponent = nullptr;

		Core::Delegate::FDelegateHandle OnPositionChangedHandle;
		Core::Delegate::FDelegateHandle OnRotationChangedHandle;

		Graphics::SpotLight mSpotLightData;
		Graphics::SpotShadowEffect mSpotShadowEffect;
		DepthMapResolution mDepthMapResolution = DepthMapResolution::DMPR_1024;
		LightMode mLightMode = LightMode::PseudoBaked;
		bool mIsRegisteredWithRenderService = false;

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

	};
}