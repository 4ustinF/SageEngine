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

		void SetPosition(const Math::Vector3& newPosition);
		void SetDirection(const Math::Vector3& newDirection);
		void SetInnerConeAngle(float innerConeAngle);
		void SetOuterConeAngle(float outerConeAngle);
		void SetRange(float range);

		void SetAttenuation(const Math::Vector3& newAttenuation);
		void SetAttenuationConstantTerm(float constantTerm);	// Doesn't involve distance at all. It is just added flatly regardless of distance.
		void SetAttenuationLinearTerm(float linearTerm);		// Falloff proportional to distance. A straight, gentle fade.
		void SetAttenuationQuadraticTerm(float quadraticTerm);	// Falloff proportional to distance²

		void SetAmbient(const Graphics::Color& color);
		void SetDiffuse(const Graphics::Color& color);
		void SetSpecular(const Graphics::Color& color);

	private:
		void OnTransformPositionChanged(const Math::Vector3& position);
		void OnTransformRotationChanged(const Math::Quaternion& rotation);

		TransformComponent* mTransformComponent = nullptr;

		Core::Delegate::FDelegateHandle OnPositionChangedHandle;
		Core::Delegate::FDelegateHandle OnRotationChangedHandle;

		//Math::Vector3 position;
		//float range;
		//Math::Vector3 direction;
		//float innerConeAngle;
		//Math::Vector3 attenuation; // {1.0f, 0.045f, 0.0075f}
		//float outerConeAngle;
		//Color ambient;
		//Color diffuse;
		//Color specular;
	};
}