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

	////const Vector3 ballPos = Vector3(1.75f, 10.0f, 0.0f);
	//const Vector3 ballPos = Vector3(0.0f, 10.0f, 0.0f);
	//const float ballRadius = 1.0f;
	//mPhysicsObject1 = new RBPhysicsObject(new BoundingSphere(ballPos, ballRadius), 1.0f);
	////mPhysicsObject1 = new RBPhysicsObject(new BoundingSphere(Vector3(0.0f, 0.0f, 0.0f), 1.0f), Vector3(0.0f, 0.0f, 1.141f / 2.0f));
	////mPhysicsObject2 = new RBPhysicsObject(new BoundingSphere(Vector3(1.414f / 2.0f * 7.0f, 0.0f, 1.414f / 2.0f * 7.0f), 1.0f), Vector3(-1.414f / 2.0f, 0.0f, -1.414f / 2.0f));
	//mPhysicsWorld.AddObject(*mPhysicsObject1);

	//mPhysicsObject2 = new RBPhysicsObject(new BoundingBox(Vector3(0.0f, 3.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f)), 0.0f);
	////mPhysicsObject2 = new RBPhysicsObject(new BoundingSphere(Vector3(0.0f, 3.0f, 0.0f), ballRadius), 0.0f);
	//mPhysicsWorld.AddObject(*mPhysicsObject2);
}

void RBPhysicsService::Terminate()
{
	mPhysicsWorld.Clear();
}

void RBPhysicsService::Update(float deltaTime)
{
	mPhysicsWorld.Update(deltaTime);
	//DebugInput();
}

void RBPhysicsService::Render()
{
	//SimpleDraw::AddSphere(Vector3(0.0f, 8.0f, 0.0f), 32, 32, 8, Colors::Blue); // Render Dome

	mPhysicsWorld.DebugDraw(); // TODO: Move back to DebugUI
}

void RBPhysicsService::DebugUI()
{
	ImGui::Checkbox("Render Physics##RBPhysics", &mRenderDebugUI);
	mPhysicsWorld.DebugUI();
}

void RBPhysicsService::DebugInput() // TODO: Remove.
{
	//RBPhysicsObject& physicsObject = mPhysicsWorld.GetPhysicsObject(0);

	float force = 50.0f;
	float torque = 1.0f;
	auto inputSystem = Input::InputSystem::Get();
	if (inputSystem->IsKeyDown(Input::KeyCode::NUMPAD1)) // Torque in x axis
	{
		mPhysicsWorld.GetPhysicsObject(0).ApplyTorque(Vector3(torque, 0.0f, 0.0f));
	}

	if (inputSystem->IsKeyDown(Input::KeyCode::NUMPAD2)) // Torque in y axis
	{
		mPhysicsWorld.GetPhysicsObject(0).ApplyTorque(Vector3(0.0f, torque, 0.0f));
	}

	if (inputSystem->IsKeyDown(Input::KeyCode::NUMPAD3)) // Torque in z axis
	{
		mPhysicsWorld.GetPhysicsObject(0).ApplyTorque(Vector3(0.0f, 0.0f, torque));
	}

	if (inputSystem->IsKeyDown(Input::KeyCode::NUMPAD4)) // Force in x axis
	{
		mPhysicsWorld.GetPhysicsObject(0).ApplyForce(Vector3(force, 0.0f, 0.0f));
	}

	if (inputSystem->IsKeyDown(Input::KeyCode::NUMPAD5)) // Force in y axis
	{
		mPhysicsWorld.GetPhysicsObject(0).ApplyForce(Vector3(0.0f, force, 0.0f));
	}

	if (inputSystem->IsKeyDown(Input::KeyCode::NUMPAD6)) // Force in z axis
	{
		mPhysicsWorld.GetPhysicsObject(0).ApplyForce(Vector3(0.0f, 0.0f, force));
	}
}