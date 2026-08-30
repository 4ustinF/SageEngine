#pragma once

#include "SAGE/Inc/SAGE.h"

enum class ComponentId
{
	PlayerController = static_cast<uint32_t>(SAGE::ComponentId::Count),
	DoorITV,
	HealthChargerHITV,
	TextureAnimator,
	ToggleTriggerVolume,
};

//enum class ServiceId
//{
//	Clouds = static_cast<uint32_t>(SAGE::ServiceId::Count),
//};
