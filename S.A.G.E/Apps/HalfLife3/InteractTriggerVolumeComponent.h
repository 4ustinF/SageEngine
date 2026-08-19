#pragma once

#include "TypeIds.h"

class InteractTriggerVolumeComponent : public SAGE::Component
{
public:
	void LoadComponentFromTemplate(const rapidjson::Value& value) override;
	void SaveComponentToTemplate(rapidjson::Value& compObj, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) override;

	void Initialize() override;
	void Terminate() override;

	void OnTriggerStay(SAGE::RBPhysics::Collider* collider) override;

protected:
	virtual void OnInteract() {};

	SAGE::Input::InputSystem* mInputSystem = nullptr;

	bool mCanInteract = true;
};