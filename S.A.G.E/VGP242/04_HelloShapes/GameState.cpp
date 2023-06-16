#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
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

	//Top Triangle
	mVertices.push_back({ Vector3{ 0.0f, 1.0f, 0.0f }, Colors::Yellow }); //Top
	mVertices.push_back({ Vector3{ 0.5f, 0.0f, 0.0f }, Colors::Yellow }); //Bot Right
	mVertices.push_back({ Vector3{ -0.5f, 0.0f, 0.0f }, Colors::Yellow }); //Bot Left

	//Bot Right Triangle
	mVertices.push_back({ Vector3{ 0.5f, 0.0f, 0.0f }, Colors::Yellow }); //Top
	mVertices.push_back({ Vector3{ 1.0f, -1.0f, 0.0f }, Colors::Yellow }); //Bot Right
	mVertices.push_back({ Vector3{ 0.0f, -1.0f, 0.0f }, Colors::Yellow }); //Bot Left

	//Bot Left Triangle
	mVertices.push_back({ Vector3{ -0.5f, 0.0f, 0.0f }, Colors::Yellow }); //Top
	mVertices.push_back({ Vector3{ 0.0f, -1.0f, 0.0f }, Colors::Yellow }); //Bot Right
	mVertices.push_back({ Vector3{ -1.0f, -1.0f, 0.0f }, Colors::Yellow }); //Bot Left

	mMeshBuffer.Initialize(mVertices);
	mVertexShader.Initialize<VertexPC>(L"../../Assets/Shaders/DoSomething.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/DoSomething.fx");
}

void GameState::Terminate()
{
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshBuffer.Terminate();
}

void GameState::Update(float deltaTime)
{
	auto inputSystems = Input::InputSystem::Get();
	if (inputSystems->IsKeyPressed(KeyCode::TWO)) {
		MainApp().ChangeState("HeartState");
	}
	if (inputSystems->IsKeyPressed(KeyCode::THREE)) {
		MainApp().ChangeState("SwordState");
	}
}

void GameState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	mMeshBuffer.Render();
}

void GameState::DebugUI()
{

}


// Add 04_HelloShapes project
// - Copy code from 03_HelloTriangle
// - Update to use DoSomething.fx shaders
// - Create multiple meshes and vertex buffers
//	- Draw a heart
//	- Draw a triforce
//	- Draw something with more than 5 triangles
// - Use input check so you can change between them