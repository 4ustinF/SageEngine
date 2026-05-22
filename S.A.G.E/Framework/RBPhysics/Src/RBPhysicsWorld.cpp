#include "Precompiled.h"
#include "RBPhysicsWorld.h"

#include "IntersectData.h"

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
	HandleCollisions();
}

void RBPhysicsWorld::DebugDraw()
{
	//if (!mShowDebugLines) { return; }

	if (mFillDebugShapes)
	{
		for (RBPhysicsObject& object : mObjects)
		{
			auto BP = (BoundingSphere&)object.GetCollider();
			const float radius = BP.GetRadius();
			SimpleDraw::AddFilledSphere(object.GetPosition(), 16, 16, radius, Colors::Red);
		}
	}
	else
	{
		for (RBPhysicsObject& object : mObjects)
		{
			auto BP = (BoundingSphere&)object.GetCollider();
			const float radius = BP.GetRadius();
			SimpleDraw::AddSphere(object.GetPosition(), 16, 16, radius, Colors::Red);
		}
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
	for (RBPhysicsObject& object : mObjects)
	{
		object.Integrate(deltaTime);
	}
}

void RBPhysicsWorld::HandleCollisions()
{
	const int objectsCount = GetObjectsCount();
	for (int primaryIndex = 0; primaryIndex < objectsCount; ++primaryIndex)
	{
		RBPhysicsObject& primaryObject = mObjects[primaryIndex];
		for (int secondaryIndex = primaryIndex + 1; secondaryIndex < objectsCount; ++secondaryIndex)
		{
			RBPhysicsObject& secondaryObject = mObjects[secondaryIndex];
			IntersectData intersectData = primaryObject.GetCollider().Intersect(secondaryObject.GetCollider());

			if (intersectData.GetDoesIntersect())
			{
				primaryObject.SetVelocity(-primaryObject.GetVelocity());
				secondaryObject.SetVelocity(-secondaryObject.GetVelocity());
			}
		}
	}
}