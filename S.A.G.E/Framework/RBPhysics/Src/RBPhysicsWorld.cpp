#include "Precompiled.h"
#include "RBPhysicsWorld.h"

#include "IntersectData.h"

#include <Input/Inc/Input.h> // TODO: Remove

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
	//HandleCollisions();
}

void RBPhysicsWorld::DebugDraw()
{
	//if (!mShowDebugLines) { return; }

	bool isRed = true;
	if (mFillDebugShapes)
	{
		for (RBPhysicsObject& object : mObjects)
		{
			auto BP = (BoundingSphere&)object.GetCollider();
			const float radius = BP.GetRadius();
			const Color color = isRed ? Colors::Red : Colors::Blue;
			isRed = !isRed;
			SimpleDraw::AddFilledSphere(object.GetPosition(), 16, 16, radius, color);
		}
	}
	else
	{
		for (RBPhysicsObject& object : mObjects)
		{
			auto BP = (BoundingSphere&)object.GetCollider();
			const float radius = BP.GetRadius();
			const Color color = isRed ? Colors::Red : Colors::Blue;
			isRed = !isRed;
			SimpleDraw::AddSphere(object.GetPosition(), 16, 16, radius, color);
		}
	}
}

void RBPhysicsWorld::DebugUI()
{
	ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat3("Gravity", &mSettings.gravity.x, 0.1f, -10.0f, 10.0f);

	Vector3 acc = mObjects[0].GetAcceleration();
	Vector3 vel = mObjects[0].GetVelocity();
	Quaternion orientation = mObjects[0].GetOrientation();
	ImGui::DragFloat3("Acc", &acc.x, 0.1f, -1000.0f, 1000.0f); // TODO: Remove
	ImGui::DragFloat3("Vel", &vel.x, 0.1f, -1000.0f, 1000.0f); // TODO: Remove
	ImGui::DragFloat4("Orientation", &orientation.x, 0.01f, -10000.0f, 10000.0f); // TODO: Remove

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
		// Gravity
		object.ApplyForce(mSettings.gravity * object.GetMass() * deltaTime);

		object.SetPosition(object.GetPosition() + object.GetVelocity() * deltaTime);
		object.SetVelocity(object.GetVelocity() + object.GetAcceleration() * deltaTime);

		object.SetAngularVelocity(object.GetAngularVelocity() + object.GetAngularAcceleration() * deltaTime);

		Quaternion deltaOrientation = object.GetOrientation() * 0.5f * deltaTime * Quaternion(0.0f, object.GetAngularVelocity().x, object.GetAngularVelocity().y, object.GetAngularVelocity().z);
		object.SetOrientation(Normalize(object.GetOrientation() + deltaOrientation));
		
		object.Integrate(deltaTime); // TODO: Move logic into integrate
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
				const Vector3 direction = Normalize(intersectData.GetDirection());
				const Vector3 primVel = primaryObject.GetVelocity();
				const Vector3 otherDirection = Reflect(direction, Normalize(primVel));

				primaryObject.SetVelocity(Reflect(primVel, otherDirection));
				secondaryObject.SetVelocity(Reflect(secondaryObject.GetVelocity(), direction));
			}
		}
	}
}