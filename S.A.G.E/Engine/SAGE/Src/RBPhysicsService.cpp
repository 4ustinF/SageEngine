#include "Precompiled.h"
#include "RBPhysicsService.h"

#include "GameObject.h"
#include "GameWorld.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;

void RBPhysicsService::Initialize()
{
	SetServiceName("RBPhysics Service");
}

void RBPhysicsService::Terminate()
{

}

void RBPhysicsService::Update(float deltaTime)
{

}

void RBPhysicsService::DebugUI()
{
	ImGui::Checkbox("Render Physics##RBPhysics", &mRenderDebugUI);

}

void RBPhysicsService::SetGravity(float gravity)
{
	mGravity = gravity;
}
