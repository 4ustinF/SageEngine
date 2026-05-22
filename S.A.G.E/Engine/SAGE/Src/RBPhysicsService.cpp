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

	// Set up object first
	mPhysicsObject1.SetPosition(Vector3(-5.0f, 0.0f, 0.0f));
	mPhysicsObject1.SetVelocity(Vector3(1.0f, 0.0f, 0.0f));
	mPhysicsObject1.SetRadius(1.0f);

	// Set up second object
	mPhysicsObject2.SetPosition(Vector3(5.0f, 0.0f, 0.0f));
	mPhysicsObject2.SetVelocity(Vector3(-1.0f, 0.0f, 0.0f));
	mPhysicsObject2.SetRadius(2.0f);

	mPhysicsWorld.AddObject(mPhysicsObject1);
	mPhysicsWorld.AddObject(mPhysicsObject2);
}

void RBPhysicsService::Terminate()
{
	mPhysicsWorld.Clear();
}

void RBPhysicsService::Update(float deltaTime)
{
	mPhysicsWorld.Update(deltaTime);
	mPhysicsWorld.DebugDraw(); // TODO: Move out.
}

void RBPhysicsService::DebugUI()
{
	ImGui::Checkbox("Render Physics##RBPhysics", &mRenderDebugUI);
	//mPhysicsWorld.DebugDraw();
	mPhysicsWorld.DebugUI();
}
