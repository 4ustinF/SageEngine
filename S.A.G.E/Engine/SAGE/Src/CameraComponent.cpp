#include "Precompiled.h"
#include "CameraComponent.h"

#include "CameraService.h"
#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;

MEMORY_POOL_DEFINE(CameraComponent, 140);

void CameraComponent::Initialize()
{
	auto cameraService = GetOwner().GetWorld().GetService<CameraService>();
	cameraService->Register(this);
}

void CameraComponent::Terminate()
{
	auto cameraService = GetOwner().GetWorld().GetService<CameraService>();
	cameraService->Unregister(this);
}