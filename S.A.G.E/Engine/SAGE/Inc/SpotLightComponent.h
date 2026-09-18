#pragma once

#include "Component.h"

namespace SAGE
{
	class TransformComponent;

	class SpotLightComponent final : public Component
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
		void OnTransformPositionChanged(const Math::Vector3& position);
		void OnTransformRotationChanged(const Math::Quaternion& rotation);

		TransformComponent* mTransformComponent = nullptr;

		Core::Delegate::FDelegateHandle OnPositionChangedHandle;
		Core::Delegate::FDelegateHandle OnRotationChangedHandle;

		float mInnerConeAngle = 10.0f; // 10.0f * Constants::DegToRad = 0.17453292519f
		float mOuterConeAngle = 50.0f; // 50.0f * Constants::DegToRad = 0.87266462599f
		float mRange = 100.0f;
		Math::Vector3 mAttenuation = Math::Vector3(1.0f, 0.045f, 0.0075f);
		Graphics::Color mAmbientColor = { 0.05f, 0.05f, 0.05f, 1.0f };
		Graphics::Color mDiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		Graphics::Color mSpecularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
}