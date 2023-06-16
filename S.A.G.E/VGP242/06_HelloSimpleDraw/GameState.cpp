#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({0.0f, 3.0f, -3.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});
}

void GameState::Terminate()
{
}

void GameState::Update(float deltaTime)
{
	auto inputSystem = InputSystem::Get();

	const float moveSpeed = inputSystem->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 1.0f;
	const float turnSpeed = 0.1f;
	if (inputSystem->IsKeyDown(KeyCode::W))
		mCamera.Walk(moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::S))
		mCamera.Walk(-moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::D))
		mCamera.Strafe(moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::A))
		mCamera.Strafe(-moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::Q))
		mCamera.Rise(moveSpeed * deltaTime);
	if (inputSystem->IsKeyDown(KeyCode::E))
		mCamera.Rise(-moveSpeed * deltaTime);

	if (inputSystem->IsMouseDown(MouseButton::RBUTTON)) {
		mCamera.Yaw(inputSystem->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(inputSystem->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}

void GameState::Render()
{
	//AABB
	//SimpleDraw::AddAABB(Math::AABB{}, mColor);
	//SimpleDraw::AddAABB(Math::Vector3{0.0f, 0.0f, 0.0f }, Math::Vector3{1.0f, 1.0f, 1.0f}, mColor);
	//SimpleDraw::AddAABB(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, mColor);
	//SimpleDraw::AddFilledAABB(Math::AABB{}, mColor);
	//SimpleDraw::AddFilledAABB(Math::Vector3{ 0.0f, 0.0f, 0.0f }, Math::Vector3{ 1.0f, 1.0f, 1.0f }, mColor);

	//Sphere
	//SimpleDraw::AddSphere(Math::Sphere{}, mColor);
	//SimpleDraw::AddSphere(Vector3{ 0.0f, 0.0f , 0.0f }, 15, 15, 1.0f, mColor);
	//SimpleDraw::AddFilledSphere(Math::Sphere{}, mColor);
	//SimpleDraw::AddFilledSphere(Vector3{ 0.0f, 0.0f , 0.0f }, 15, 15, 1.0f, mColor);

	//Plane
	//SimpleDraw::AddPlane(20, Colors::White);

	//Cylinder
	//SimpleDraw::AddCylinder(Math::Cylinder{}, mColor, false);
	//SimpleDraw::AddCylinder(Vector3{ 0.0f, 0.0f , 0.0f }, 15, 2, 1.0f, 2.0f, mColor, true);
	//SimpleDraw::AddFilledCylinder(Math::Cylinder{}, mColor, false);
	//SimpleDraw::AddFilledCylinder(Vector3{ 0.0f, 0.0f, 0.0f }, 15, 2, 1.0f, 2.0f, mColor, true);

	//Pyramid
	//SimpleDraw::AddPyramid(Math::Pyramid{}, mColor);
	//SimpleDraw::AddPyramid(Vector3{ 0.0f, 0.0f , 0.0f }, 1.0f, 1.0f, 1.0f, mColor);
	//SimpleDraw::AddFilledPyramid(Math::Pyramid{}, mColor);
	//SimpleDraw::AddFilledPyramid(Vector3{ 0.0f, 0.0f , 0.0f }, 1.0f, 1.0f, 1.0f, mColor);

	SimpleDraw::AddTransform(Matrix4::Identity);

	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::DragFloat3("Position", &mPosition.x, 0.01f);
	ImGui::ColorEdit4("Color", &mColor.r);
	ImGui::End();
}