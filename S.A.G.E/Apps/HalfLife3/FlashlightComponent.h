#pragma once

#include "TypeIds.h"

class SAGE::CameraService;
class SAGE::Input::InputSystem;
class SAGE::SpotlightComponent;
class SAGE::TransformComponent;

class FlashlightComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::Flashlight);
	MEMORY_POOL_DECLARE;

	const char* GetCompName() override { return "Flashlight Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

private:
	SAGE::CameraService* mCameraService = nullptr;
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::SpotlightComponent* mSpotlightComponent = nullptr;
	SAGE::TransformComponent* mTransformComponent = nullptr;

};