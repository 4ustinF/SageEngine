#pragma once
#include "InteractTriggerVolumeComponent.h"

class DoorITVComponent final : public InteractTriggerVolumeComponent
{
public:
	SET_TYPE_ID(ComponentId::DoorITV);
	MEMORY_POOL_DECLARE;
	
	const char* GetCompName() override { return "Door ITV Component"; }
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;
	void DebugUI() override;
	
protected:
	void OnInteract() override;

private:
	SAGE::GameObject* GetDoorObject();
	SAGE::GameObject* mDoorObject = nullptr;
};