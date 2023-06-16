#pragma once

#include "SAGE/Inc/SAGE.h"

enum class ComponentId
{
	Animal = static_cast<uint32_t>(SAGE::ComponentId::Count),
	PlayerController,
	PlayerUI,
	WorldGenerator, 
	VoxelCameraController,
};

enum class ServiceId
{
	Clouds = static_cast<uint32_t>(SAGE::ServiceId::Count),
	Mob,
	PlayerUI,
	World,
};
