#pragma once

#include "RBPhysicsObject.h"

namespace SAGE::RBPhysics
{
	class RBPhysicsWorld
	{
	public:
		struct Settings
		{
			Math::Vector3 gravity{ 0.0f, -9.81f, 0.0f };
			float drag = 0.0f;
			float maxAirdrag = 1.0f;
			float airDragCoeficient = 0.05f;
			float bounceCoeficient = 0.5f; // 0 = no bounce, 1 = perfect bounce
		};
		RBPhysicsWorld() = default;

		void Initialize(Settings settings = {});
		void Clear();

		void Update(float deltaTime);
		void DebugDraw();
		void DebugUI();

		void ShowDebugLines(bool showDebug) { mShowDebugLines = showDebug; }

		int AddObject(const RBPhysicsObject& object);

		// TODO: These are temp please remove.
		RBPhysicsObject& GetPhysicsObject(int index)
		{
			return mObjects[index];
		}

		// TODO: These are temp please remove.
		int GetObjectsCount() const
		{
			return static_cast<int>(mObjects.size());
		}

		// TODO: These are temp please remove.
		RBPhysicsObject& GetStaticPhysicsObject(int index)
		{
			return mStaticObjects[index];
		}

		// TODO: These are temp please remove.
		int GetStaticObjectsCount() const
		{
			return static_cast<int>(mStaticObjects.size());
		}

	private:
		void Simulate(float deltaTime);
		void HandleCollisions();

		std::vector<RBPhysicsObject> mObjects;
		std::vector<RBPhysicsObject> mStaticObjects;

		Settings mSettings;
		bool mShowDebugLines = true;
		bool mFillDebugShapes = false;

		void DetectCollisionWithDome(float deltaTime);
		void ResolveCollisionWithDome(RBPhysicsObject& object, float deltaTime);
		Math::Vector3 GetVelocityAtPoint(const RBPhysicsObject& object, const Math::Vector3& localPoint);

		void ResolveCollision(RBPhysicsObject& object1, RBPhysicsObject& object2, IntersectData& intersectData);
	};
}