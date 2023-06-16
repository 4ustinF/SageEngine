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

	//-------------------------------------------------------
	mCubeMesh = MeshBuilder::CreateCubePC();
	mCubeMeshBuffer.Initialize(mCubeMesh);

	mPlaneMesh = MeshBuilder::CreatePlanePC(2, 2);
	mPlaneMeshBuffer.Initialize(mPlaneMesh);

	mCylinderMesh = MeshBuilder::CreateCylinderPC(30, 2);
	mCylinderMeshBuffer.Initialize(mCylinderMesh);

	mSphereMesh = MeshBuilder::CreateSpherePC(30, 30, 1);
	mSphereMeshBuffer.Initialize(mSphereMesh);

	mVertexShader.Initialize<VertexPC>(L"../../Assets/Shaders/DoTransform.fx");
	mPixelShader.Initialize(L"../../Assets/Shaders/DoTransform.fx");

	mConstantBuffer.Initialize(sizeof(Matrix4));
}

void GameState::Terminate()
{
	mConstantBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();

	mSphereMeshBuffer.Terminate();
	mCylinderMeshBuffer.Terminate();
	mPlaneMeshBuffer.Terminate();
	mCubeMeshBuffer.Terminate();
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
	//Matrix4 wvp = Transpose(world * view * proj);

	//mConstantBuffer.Update(&wvp);
	mConstantBuffer.BindVS(0);

	Matrix4 cubeWorld = Matrix4::Identity * Matrix4::Translation({-4.0f, 0.0f, 3.0f});
	Matrix4 planeWorld = Matrix4::Identity * Matrix4::Translation({ -1.0f, -1.0f, 0.0f });
	Matrix4 cylinderWorld = Matrix4::Identity * Matrix4::Translation({ 1.0f, 0.0f, 3.0f });
	Matrix4 sphereWorld = Matrix4::Identity * Matrix4::Translation({ 4.0f, 0.0f, 3.0f });
	
	Matrix4 wvp = Transpose(cubeWorld * view * proj);
	mConstantBuffer.Update(&wvp);
	mCubeMeshBuffer.Render();

	wvp = Transpose(planeWorld * view * proj);
	mConstantBuffer.Update(&wvp);
	mPlaneMeshBuffer.Render();

	wvp = Transpose(cylinderWorld * view * proj);
	mConstantBuffer.Update(&wvp);
	mCylinderMeshBuffer.Render();

	wvp = Transpose(sphereWorld * view * proj);
	mConstantBuffer.Update(&wvp);
	mSphereMeshBuffer.Render();
}

void GameState::DebugUI()
{

}