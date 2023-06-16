#pragma once

#include "Common.h"

//App Headers
#include "App.h"
#include "AppState.h"

// Component Headers
#include "AnimatorComponent.h"
#include "BPhysicsComponent.h"
#include "CameraComponent.h"
#include "ColliderComponent.h"
#include "FollowCameraControllerComponent.h"
#include "FPSCameraControllerComponent.h"
#include "ModelComponent.h"
#include "ParticleComponent.h"
#include "RigidBodyComponent.h"
#include "SpawnerComponent.h"
#include "TransformComponent.h"

// Service Headers
#include "BPhysicsService.h"
#include "CameraService.h"
#include "PostProcessingService.h"
#include "ParticleService.h"
#include "PhysicsService.h"
#include "RenderService.h"
#include "TerrainService.h"
#include "UIService.h"

// World Headers
#include "Component.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "GameObjectHandle.h"
#include "GameWorld.h"
#include "Service.h"

//Other
#include "CameraUtil.h"

namespace SAGE
{
	App& MainApp();
}