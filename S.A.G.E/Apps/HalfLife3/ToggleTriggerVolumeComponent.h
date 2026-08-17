#pragma once

#include "TypeIds.h"

class ToggleTriggerVolumeComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::ToggleTriggerVolume);
	MEMORY_POOL_DECLARE;

	const char* GetCompName() override { return "Toggle Trigger Volume Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Terminate() override;

	void DebugUI() override;

	void OnTriggerEnter(SAGE::RBPhysics::Collider* collider) override;

private:
	std::vector<std::string> mObjectsToDisableList;
	std::vector<std::string> mObjectsToEnableList;
};