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
			float timeStep = 1.0f / 60.0f;
			float drag = 0.0f;
			float maxAirdrag = 1.0f;
			float airDragCoeficient = 0.05f;
			int iterations = 1;
		};
		RBPhysicsWorld() = default;

		void Initialize(Settings settings = {});
		void Clear();

		void Update(float deltaTime);
		void DebugDraw();
		void DebugUI();

		void ShowDebugLines(bool showDebug) { mShowDebugLines = showDebug; }

		// RBPhysicsWorld.h
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

	private:
		void Simulate(float deltaTime);
		void HandleCollisions();

		std::vector<RBPhysicsObject> mObjects;

		Settings mSettings;
		bool mShowDebugLines = true;
		bool mFillDebugShapes = false;

		void DetectCollisionWithDome(float deltaTime);
		void ResolveCollisionWithDome(RBPhysicsObject& object, float deltaTime);
		Math::Vector3 GetVelocityAtPoint(const RBPhysicsObject& object, const Math::Vector3& localPoint);
	};
}