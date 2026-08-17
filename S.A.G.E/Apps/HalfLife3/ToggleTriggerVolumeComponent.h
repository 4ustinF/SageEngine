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

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void OnTriggerEnter(SAGE::RBPhysics::Collider* collider) override;
	void OnTriggerStay(SAGE::RBPhysics::Collider* collider) override;
	void OnTriggerExit(SAGE::RBPhysics::Collider* collider) override;

private:

};