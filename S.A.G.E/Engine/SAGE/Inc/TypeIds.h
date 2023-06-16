#pragma once

#include "Common.h"

namespace SAGE
{
	enum class ComponentId
	{
		Animator,
		BPhysics,
		Camera,
		Collider,
		FollowCameraController,
		FPSCameraController,
		Model,
		Particle,
		RigidBody,
		Spawner,
		Transform,
		Count
	};

	enum class ServiceId
	{
		Camera,
		Particles,
		BPhysics,
		Physics,
		PostProcessing,
		Render,
		Terrain,
		UserInterface,
		Count
	};

	enum CollisionGroups
	{
		Default = 1 << 0,
		Group1 = 1 << 1,
		Group2 = 1 << 2,
		Group3 = 1 << 3,
		Group4 = 1 << 4,
		Group5 = 1 << 5,
		Group6 = 1 << 6,
		Group7 = 1 << 7,
		Group8 = 1 << 8,
		Group9 = 1 << 9
	};
}

#define SET_TYPE_ID(id)\
	static uint32_t StaticGetTypeId() { return static_cast<uint32_t>(id); }\
	uint32_t GetTypeId() const override { return StaticGetTypeId(); }