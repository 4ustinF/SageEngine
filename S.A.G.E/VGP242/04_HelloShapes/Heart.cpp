#include "HeartState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void HeartState::Initialize()
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

	//Bot
	mVertices.push_back({ Vector3{ -0.75f, 0.0f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.75f, 0.0f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.0f, -1.0f, 0.0f }, Colors::Red });

	//bot Square
	mVertices.push_back({ Vector3{ -0.75f, 0.0f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ -0.75f, 0.25f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.75f, 0.0f, 0.0f }, Colors::Red });

	//Top Square
	mVertices.push_back({ Vector3{ -0.75f, 0.25f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.75f, 0.25f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.75f, 0.0f, 0.0f }, Colors::Red });

	//Top Right
	mVertices.push_back({ Vector3{ 0.0f, 0.25f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.375f, 1.0f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.75f, 0.25f, 0.0f }, Colors::Red });

	//Top Left
	mVertices.push_back({ Vector3{ -0.75f, 0.25f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ -0.375f, 1.0f, 0.0f }, Colors::Red });
	mVertices.push_back({ Vector3{ 0.0f, 0.25f, 0.0f }, Colors::Red });

	mMeshBuffer.Initialize(mVertices);
	mVertexShader.Initialize<VertexPC>(L"../../Assets/Shaders/DoSomething.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/DoSomething.fx");
}

void HeartState::Terminate()
{
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshBuffer.Terminate();
}

void HeartState::Update(float deltaTime)
{
	auto inputSystems = Input::InputSystem::Get();
	if (inputSystems->IsKeyPressed(KeyCode::ONE)) {
		MainApp().ChangeState("GameState");
	}
	if (inputSystems->IsKeyPressed(KeyCode::THREE)) {
		MainApp().ChangeState("SwordState");
	}
}

void HeartState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	mMeshBuffer.Render();
}

void HeartState::DebugUI()
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