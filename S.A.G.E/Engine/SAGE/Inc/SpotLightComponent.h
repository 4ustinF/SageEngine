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

	private:
		void OnTransformPositionChanged(const Math::Vector3& position);
		void OnTransformRotationChanged(const Math::Quaternion& rotation);

		TransformComponent* mTransformComponent = nullptr;

		Core::Delegate::FDelegateHandle OnPositionChangedHandle;
		Core::Delegate::FDelegateHandle OnRotationChangedHandle;
	};
}