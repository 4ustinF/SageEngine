#pragma once

#include "SAGE/Inc/SAGE.h"

enum class ComponentId
{
	BirdControllerComponent = static_cast<uint32_t>(SAGE::ComponentId::Count),
	PipeComponent,
};

enum class ServiceId
{
	//GameManagerService = static_cast<uint32_t>(SAGE::ServiceId::Count),
};
