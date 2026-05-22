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

void RBPhysicsWorld::Update(float deltaTime)
{
	if (mPause) { return; }

	mTimer += deltaTime;
	while (mTimer >= mSettings.timeStep)
	{
		mTimer -= mSettings.timeStep;
	}
}

void RBPhysicsWorld::DebugDraw() const
{
	if (!mShowDebugLines) { return; }

}

void RBPhysicsWorld::DebugUI()
{
	ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat3("Gravity", &mSettings.gravity.x, 0.1f, -10.0f, 10.0f);
	ImGui::Checkbox("Pause simulation", &mPause);
	ImGui::Checkbox("Show Debug Lines", &mShowDebugLines);
	ImGui::Checkbox("Fill Debug Shapes", &mFillDebugShapes);
	ImGui::End();
}

