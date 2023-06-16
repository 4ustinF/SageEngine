#include "Precompiled.h"
#include "PhysicsService.h"

using namespace SAGE;
using namespace SAGE::Physics;

void PhysicsService::Initialize()
{
	SetServiceName("Physics Service");
	mPhysicsWorld.Initialize();
}

void PhysicsService::Terminate()
{
	mPhysicsWorld.Clear(true);
}

void PhysicsService::Update(float deltaTime)
{
	mPhysicsWorld.Update(deltaTime);
}

void PhysicsService::Render()
{
	mPhysicsWorld.DebugDraw();
}

void PhysicsService::DebugUI()
{
	mPhysicsWorld.DebugUI();
}
