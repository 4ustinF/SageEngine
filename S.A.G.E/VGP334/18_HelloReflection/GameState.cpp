#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	//GraphicsSystem::Get()->SetClearColor(Colors::SteelBlue);

	mCamera.SetPosition({0.0f, 3.0f, -5.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000023f);
	mStandardEffect.SetBumpWeight(0.25f);
	mStandardEffect.SetSampleSize(0);

	mReflectionEffect.Initialize();
	mReflectionEffect.SetCamera(mCamera);
	mReflectionEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mReflectionEffect.SetDirectionalLight(mDirectionalLight);
	mReflectionEffect.SetSourceTexture(mBaseRenderTarget);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);
	mShadowEffect.SetSize(50.0f);

	mCube.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCube.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mCube.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCube.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mCube.material.power = 10.0f;
	mCube.diffuseMapId = TextureManager::Get()->LoadTexture("earth.jpg");
	mCube.bumpMapId = TextureManager::Get()->LoadTexture("earth_bump.jpg");
	mCube.normalMapId = TextureManager::Get()->LoadTexture("earth_normal.jpg");
	mCube.specularMapId = TextureManager::Get()->LoadTexture("earth_spec.jpg");
	mCube.meshBuffer.Initialize(MeshBuilder::CreateSphere(24, 24, 1.0f));

	mCube.transform.position.y = 3.0f;

	mFloor.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mFloor.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mFloor.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mFloor.material.emissive = { 0.1f, 0.1f, 0.1f, 1.0f };
	mFloor.material.power = 10.0f;
	mFloor.diffuseMapId = TextureManager::Get()->LoadTexture("concrete.jpg");
	mFloor.meshBuffer.Initialize(MeshBuilder::CreatePlane(25, 25, 1.0f));

	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();
	mBaseRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);

	mScreenQuad.meshBuffer.Initialize(MeshBuilder::CreateScreenQuad());
}

void GameState::Terminate()
{
	mScreenQuad.Terminate();
	mBaseRenderTarget.Terminate();

	mFloor.Terminate();
	mCube.Terminate();
	mShadowEffect.Terminate();
	mReflectionEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{	
	auto inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::M))
	{
		useMouseDebugMode = !useMouseDebugMode;
		inputSystem->ShowSystemCursor(useMouseDebugMode);
	}

	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 2.0f, 10.0f, 0.1f, deltaTime, useMouseDebugMode);
	mFPS = 1.0f / deltaTime;
}

void GameState::Render()
{
	mBaseRenderTarget.BeginRender();
	{
		mStandardEffect.Begin();
		{
			mStandardEffect.Render(mCube);
			mStandardEffect.Render(mFloor);
		}
		mStandardEffect.End();

		mShadowEffect.Begin();
		{
			mShadowEffect.Render(mFloor);
			mShadowEffect.Render(mCube);
		}
		mShadowEffect.End();
	}
	mBaseRenderTarget.EndRender();

	mReflectionEffect.Begin();
	{
		mReflectionEffect.Render(mScreenQuad);
	}
	mReflectionEffect.End();

	SimpleDraw::AddTransform(Matrix4::Identity);
	SimpleDraw::AddPlane(10, Colors::White);
	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f", mFPS);
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}

	if (ImGui::CollapsingHeader("Cube", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Cube", &mCube.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Cube", &mCube.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Cube", &mCube.material.specular.r);
		ImGui::ColorEdit4("Emissive##Cube", &mCube.material.emissive.r);
		ImGui::DragFloat("Power##Cube", &mCube.material.power, 1.0f, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Floor", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Floor", &mFloor.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Floor", &mFloor.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Floor", &mFloor.material.specular.r);
		ImGui::ColorEdit4("Emissive##Floor", &mFloor.material.emissive.r);
		ImGui::DragFloat("Power##Floor", &mFloor.material.power, 1.0f, 1.0f, 100.0f);
	}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mReflectionEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	ImGui::Text("Base");
	ImGui::Image(mBaseRenderTarget.GetRawData(), { 256, 144 });

	ImGui::End();
}