#pragma once

#include "TypeIds.h"

class SAGE::Input::InputSystem;

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
	

};