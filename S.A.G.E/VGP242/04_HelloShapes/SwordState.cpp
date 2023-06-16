#include "SwordState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void SwordState::Initialize()
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

	//Bot Triangle
	mVertices.push_back({ Vector3{ -0.1f, -0.9f, 0.0f }, Colors::Gold }); 
	mVertices.push_back({ Vector3{ 0.1f, -0.9f, 0.0f }, Colors::Gold });
	mVertices.push_back({ Vector3{ 0.0f, -1.0f, 0.0f }, Colors::Gold }); 

	//Middle Triangle
	mVertices.push_back({ Vector3{ -0.15f, -0.3f, 0.0f }, Colors::Gold });
	mVertices.push_back({ Vector3{ 0.15f, -0.3f, 0.0f }, Colors::Gold });
	mVertices.push_back({ Vector3{ 0.0f, -0.4f, 0.0f }, Colors::Gold });

	//Right Handle
	mVertices.push_back({ Vector3{ -0.025f, -0.9f, 0.0f }, Colors::Brown });
	mVertices.push_back({ Vector3{ 0.025f, -0.35f, 0.0f }, Colors::Brown });
	mVertices.push_back({ Vector3{ 0.025f, -0.9f, 0.0f }, Colors::Brown });

	//Left handle
	mVertices.push_back({ Vector3{ -0.025f, -0.35f, 0.0f }, Colors::Brown });
	mVertices.push_back({ Vector3{ 0.025f, -0.35f, 0.0f }, Colors::Brown });
	mVertices.push_back({ Vector3{ -0.025f, -0.9f, 0.0f }, Colors::Brown });

	//Right Blade
	mVertices.push_back({ Vector3{ -0.05f, -0.3f, 0.0f }, Colors::White });
	mVertices.push_back({ Vector3{ 0.05f, 0.9f, 0.0f }, Colors::White });
	mVertices.push_back({ Vector3{ 0.05f, -0.3f, 0.0f }, Colors::White });

	//Left Blade
	mVertices.push_back({ Vector3{ -0.05f, 0.6f, 0.0f }, Colors::White });
	mVertices.push_back({ Vector3{ 0.05f, 0.9f, 0.0f }, Colors::White });
	mVertices.push_back({ Vector3{ -0.05f, -0.3f, 0.0f }, Colors::White });

	mMeshBuffer.Initialize(mVertices);
	mVertexShader.Initialize<VertexPC>(L"../../Assets/Shaders/DoSomething.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/DoSomething.fx");

}

void SwordState::Terminate()
{
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshBuffer.Terminate();
}

void SwordState::Update(float deltaTime)
{
	auto inputSystems = Input::InputSystem::Get();
	if (inputSystems->IsKeyPressed(KeyCode::ONE)) {
		MainApp().ChangeState("GameState");
	}
	if (inputSystems->IsKeyPressed(KeyCode::TWO)) {
		MainApp().ChangeState("HeartState");
	}
}

void SwordState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	mMeshBuffer.Render();
}

void SwordState::DebugUI()
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