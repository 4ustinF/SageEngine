#include "Precompiled.h"
#include "RBPhysicsWorld.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::RBPhysics;

void RBPhysicsWorld::Initialize(Settings settings)
{
	mSettings = std::move(settings);
}

void RBPhysicsWorld::Clear()
{
	mObjects.clear();
}

void RBPhysicsWorld::Update(float deltaTime)
{
	Simulate(deltaTime);
}

void RBPhysicsWorld::DebugDraw()
{
	//if (!mShowDebugLines) { return; }

	for (RBPhysicsObject& object : mObjects)
	{
		float Radius = object.GetBoundingSphere().radius;
		SimpleDraw::AddSphere(object.GetPosition(), 16, 16, Radius, Colors::Red);
	}
}

void RBPhysicsWorld::DebugUI()
{
	ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat3("Gravity", &mSettings.gravity.x, 0.1f, -10.0f, 10.0f);
	ImGui::Checkbox("Show Debug Lines", &mShowDebugLines);
	ImGui::Checkbox("Fill Debug Shapes", &mFillDebugShapes);
	ImGui::End();
}

void RBPhysicsWorld::AddObject(const RBPhysicsObject& object)
{
	mObjects.push_back(object);
}

void RBPhysicsWorld::Simulate(float deltaTime)
{
	// TODO: Remove
	//for (int objectIndex = 0; objectIndex < mObjects.size(); ++objectIndex)
	//{
	//	mObjects[objectIndex].Integrate(deltaTime);
	//}

	for (RBPhysicsObject& object : mObjects)
	{
		object.Integrate(deltaTime);
	}
}