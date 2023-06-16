#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({0.0f, 1.0f, -3.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	// NDC - Normalized Device Coordinate
	//
	//          +-----------------+
	//         /                 /|
	//   1.0  +-----------------+ |
	//	      |        ^        | |
	//	      |        |        | |
	//	      | <------+------> | |
	//	      |        |        | | 1.0
	//	      |        v        |/
	//	-1.0  +-----------------+ 0.0
	//      -1.0               1.0
	//

	//Rubix cube
	
#pragma region ---WhiteFront---

	//Top
	mVertices.push_back({ Vector3{ -0.5f,  0.5f, -0.5f }, Colors::White }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, 0.5f, -0.5f  }, Colors::White }); //Top Right
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, -0.5f  }, Colors::White }); //Bot Right

	//Bot
	mVertices.push_back({ Vector3{ -0.5f, 0.5f, -0.5f  }, Colors::White }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, -0.5f  }, Colors::White }); //Bot Right
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, -0.5f  }, Colors::White }); //Bot Left

#pragma endregion

#pragma region ---YellowBack---

	//Top
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, 0.5f  }, Colors::Yellow }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, 0.5f, 0.5f  }, Colors::Yellow }); //Top Right
	mVertices.push_back({ Vector3{ -0.5f,  0.5f, 0.5f  }, Colors::Yellow }); //Bot Right

	//Bot
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, 0.5f }, Colors::Yellow }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, 0.5f }, Colors::Yellow }); //Bot Right
	mVertices.push_back({ Vector3{ -0.5f, 0.5f, 0.5f }, Colors::Yellow }); //Bot Left

#pragma endregion

#pragma region ---GreenRight---

	//Top
	mVertices.push_back({ Vector3{ 0.5f,  0.5f, -0.5f }, Colors::Green }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, 0.5f, 0.5f }, Colors::Green }); //Top Right
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, 0.5f }, Colors::Green }); //Bot Right

	//Bot
	mVertices.push_back({ Vector3{ 0.5f, 0.5f, -0.5f }, Colors::Green }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, 0.5f }, Colors::Green }); //Bot Right
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, -0.5f }, Colors::Green }); //Bot Left

#pragma endregion

#pragma region ---BlueLeft---

	//Top
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, 0.5f }, Colors::Blue }); //Bot Right
	mVertices.push_back({ Vector3{ -0.5f, 0.5f, 0.5f }, Colors::Blue }); //Top Right
	mVertices.push_back({ Vector3{ -0.5f,  0.5f, -0.5f }, Colors::Blue }); //Top Left

	//Bot
	mVertices.push_back({ Vector3{ -0.5f, 0.5f, -0.5f }, Colors::Blue }); //Top Right
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, -0.5f }, Colors::Blue }); //Bot Right
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, 0.5f }, Colors::Blue }); //Bot Left

#pragma endregion

#pragma region ---RedTop---

	//Top
	mVertices.push_back({ Vector3{ -0.5f,  0.5f, 0.5f }, Colors::Red }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, 0.5f, 0.5f }, Colors::Red }); //Top Right
	mVertices.push_back({ Vector3{ 0.5f, 0.5f, -0.5f }, Colors::Red }); //Bot Right

	//Bot
	mVertices.push_back({ Vector3{ -0.5f, 0.5f, 0.5f }, Colors::Red }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, 0.5f, -0.5f }, Colors::Red }); //Bot Right
	mVertices.push_back({ Vector3{ -0.5f, 0.5f, -0.5f }, Colors::Red }); //Bot Left

#pragma endregion

#pragma region ---OrangeBottom---

	//Top
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, -0.5f }, Colors::Orange }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, -0.5f }, Colors::Orange }); //Top Right
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, 0.5f }, Colors::Orange }); //Bot Right

	//Bot
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, -0.5f }, Colors::Orange }); //Top Left
	mVertices.push_back({ Vector3{ 0.5f, -0.5f, 0.5f }, Colors::Orange }); //Bot Right
	mVertices.push_back({ Vector3{ -0.5f, -0.5f, 0.5f }, Colors::Orange }); //Bot Left

#pragma endregion


	//-------------------------------------------------------
	mMeshBuffer.Initialize(mVertices);
	mVertexShader.Initialize<VertexPC>(L"../../Assets/Shaders/DoTransform.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/DoTransform.fx");

	mConstantBuffer.Initialize(sizeof(Matrix4));
}

void GameState::Terminate()
{
	mConstantBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshBuffer.Terminate();
}

void GameState::Update(float deltaTime)
{
	auto inputSystems = Input::InputSystem::Get();
	float rotation = 0.0f;

	if (inputSystems->IsKeyDown(KeyCode::RIGHT)) {
		rotation += mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationY(rotation);
	}
	if (inputSystems->IsKeyDown(KeyCode::LEFT)) {
		rotation -= mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationY(rotation);
	}
	if (inputSystems->IsKeyDown(KeyCode::UP)) {
		rotation += mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationX(rotation);
	}
	if (inputSystems->IsKeyDown(KeyCode::DOWN)) {
		rotation -= mCameraMoveSpeed;
		world = world * SAGE::Math::Matrix4::RotationX(rotation);
	}
}

void GameState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	Matrix4 view = mCamera.GetViewMatrix();
	Matrix4 proj = mCamera.GetProjectionMatrix();
	Matrix4 wvp = Transpose(world * view * proj);

	mConstantBuffer.Update(&wvp);
	mConstantBuffer.BindVS(0);

	mMeshBuffer.Render();
}

void GameState::DebugUI()
{

}