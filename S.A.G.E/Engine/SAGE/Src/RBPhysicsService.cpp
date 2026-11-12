#include "Precompiled.h"
#include "RBPhysicsService.h"

#include "CameraService.h"
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
	mPhysicsWorld.Clear();
}

void RBPhysicsService::Update(float deltaTime)
{
	mPhysicsWorld.Update(deltaTime);
}

void RBPhysicsService::Render()
{
	if (mRenderDebugUI)
	{
		mPhysicsWorld.DrawPhysicsObjects(mFillDebugShapes);
	}
}

void RBPhysicsService::DebugUI()
{
	ImGui::Checkbox("Render Physics##RBPhysics", &mRenderDebugUI);
	ImGui::Checkbox("Fill Debug Shapes##RBPhysics", &mFillDebugShapes);
	mPhysicsWorld.DebugUI();
}
