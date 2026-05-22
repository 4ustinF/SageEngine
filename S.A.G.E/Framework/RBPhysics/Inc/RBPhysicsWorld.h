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

		void Update(float deltaTime);
		void DebugDraw() const;
		void DebugUI();

		void Pause(bool isPaused) { mPause = isPaused; }
		void ShowDebugLines(bool showDebug) { mShowDebugLines = showDebug; }
		bool IsPaused() { return mPause; }

	private:
		void AccumulateForces();
		void Intergrate();

		Settings mSettings;
		float mTimer = 0.0f;
		bool mPause = false;
		bool mShowDebugLines = true;
		bool mFillDebugShapes = false;
	};
}