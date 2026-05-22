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
			int iterations = 1;
		};

		void Initialize(Settings settings = {});
		void Clear();

		void Update(float deltaTime);
		void DebugDraw() const;
		void DebugUI();

		void ShowDebugLines(bool showDebug) { mShowDebugLines = showDebug; }

		void AddObject(const RBPhysicsObject& object);

		// TODO: These are temp please remove.
		const RBPhysicsObject& GetObject(int index) const
		{
			return mObjects[index];
		}

		// TODO: These are temp please remove.
		int GetObjectsCount() const
		{
			return mObjects.size();
		}

	private:
		void Simulate(float deltaTime);

		std::vector<RBPhysicsObject> mObjects;

		Settings mSettings;
		bool mShowDebugLines = true;
		bool mFillDebugShapes = false;
	};
}