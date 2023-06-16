#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({0.0f, 3.0f, -5.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});
	//InputSystem::Get()->debugFPSMouse = false;

	//obb1.rotation = {};
	//obb2.rotation = {};

	aabb1.center = { -3.0f, 0.0f, 0.0f };
}

void GameState::Terminate()
{

}

void GameState::Update(float deltaTime)
{
	if (InputSystem::Get()->IsKeyPressed(KeyCode::F))
	{
		mShowFilled = !mShowFilled;
	}

	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 2.0f, 10.0f, 0.1f, deltaTime);
	mFPS = 1.0f / deltaTime;
}

void GameState::Render()
{
	Vector3 normal;
	float amt;
	if (Intersect(aabb1, aabb2, normal, amt)) {
		mShowFilled ? SimpleDraw::AddFilledAABB(aabb1, Colors::Red) : SimpleDraw::AddAABB(aabb1, Colors::Red);
	}
	else {
		mShowFilled ? SimpleDraw::AddFilledAABB(aabb1, Colors::Cyan) : SimpleDraw::AddAABB(aabb1, Colors::Cyan);
	}

	mShowFilled ? SimpleDraw::AddFilledAABB(aabb2, Colors::Cornsilk) : SimpleDraw::AddAABB(aabb2, Colors::Cornsilk);

	//SimpleDraw::AddPlane(20, Colors::White);
	SimpleDraw::AddTransform(Matrix4::Identity);
	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f", mFPS);
	ImGui::Separator();

	ImGui::DragFloat3("Move", &aabb1.center.x, 0.01f, -10.0f, 10.0f);
	if (ImGui::Button("Show debug fill"))
	{
		mShowFilled = !mShowFilled;
	}

	ImGui::End();
}