#include "Precompiled.h"
#include "RBPhysicsService.h"

#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::RBPhysics;

void RBPhysicsService::Initialize()
{
	SetServiceName("RBPhysics Service");
	mPhysicsWorld.Initialize();
}

void RBPhysicsService::Terminate()
{

}

void RBPhysicsService::Update(float deltaTime)
{
	mPhysicsWorld.Update(deltaTime);
}

void RBPhysicsService::DebugUI()
{
	ImGui::Checkbox("Render Physics##RBPhysics", &mRenderDebugUI);
	mPhysicsWorld.DebugDraw();
	mPhysicsWorld.DebugUI();
}
