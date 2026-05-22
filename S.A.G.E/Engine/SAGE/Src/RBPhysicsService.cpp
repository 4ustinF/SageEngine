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

	mPhysicsObject1 = new RBPhysicsObject(new BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 1.0f), Vector3(0.0f, 0.0f, 1.141f / 2.0f));
	mPhysicsObject2 = new RBPhysicsObject(new BoundingSphere(Vector3(1.414f / 2.0f * 7.0f, 0.0f, 1.414f / 2.0f * 7.0f), 1.0f), Vector3(-1.414f / 2.0f, 0.0f, -1.414f / 2.0f));

	mPhysicsWorld.AddObject(*mPhysicsObject1);
	mPhysicsWorld.AddObject(*mPhysicsObject2);
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
