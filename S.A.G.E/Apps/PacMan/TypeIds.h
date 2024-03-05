#pragma once

#include "SAGE/Inc/SAGE.h"

enum class ComponentId
{
	PlayerController = static_cast<uint32_t>(SAGE::ComponentId::Count),
	PlayerAnimator,
};

enum class ServiceId
{
	TileMap = static_cast<uint32_t>(SAGE::ServiceId::Count),
};
