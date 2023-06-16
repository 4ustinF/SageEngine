#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	auto inputSystem = InputSystem::Get();
	//inputSystem->isSnap = true;

	mCamera.SetPosition({0.0f, 3.0f, -10.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	mDirectionalLight.direction = Math::Normalize({1.0f, -1.0f, 1.0f});
	mDirectionalLight.ambient = { 0.0f, 0.0f, 0.0f, 1.0f};
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f};
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	//mStandardEffect.SetDepthBias(0.000023f);
	mStandardEffect.SetDepthBias(0.0001f);
	mStandardEffect.SetSampleSize(5);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);

	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();

	Model duckModel;
	ModelIO::LoadModel("../../Assets/Models/Duck/duck.model", duckModel);

	auto tm = TextureManager::Get();
	// Duck
	mDuckRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mDuckRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mDuckRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mDuckRenderObject.material.power = 10.0f;
	mDuckRenderObject.diffuseMapId = tm->LoadTexture("basketball.jpg");
	mDuckRenderObject.meshBuffer.Initialize(MeshBuilder::CreateSphere(512, 512, 1.0f));
	mDuckRenderObject.transform.position.y = 1.0f;
	//mDuckRenderObject.diffuseMapId = tm->LoadTexture("../Models/Duck/duck.png");
	//mDuckRenderObject.meshBuffer.Initialize(duckModel.meshData[0].mesh);

	// Ground
	mGroundRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mGroundRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.power = 10.0f;
	mGroundRenderObject.diffuseMapId = tm->LoadTexture("concrete.jpg");
	mGroundRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(20, 20, 1.0f));	
}

void GameState::Terminate()
{
	mGroundRenderObject.Terminate();
	mDuckRenderObject.Terminate();
	mShadowEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 1.0f, 10.0f, 0.1f, deltaTime);
	mFPS = 1.0f / deltaTime;

	auto inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::SPACE)) {
		mPause = !mPause;
	}

	const float dt = mPause ? 0.0f : deltaTime;
	mRunningTime += dt;

	//float height = mBallHeight * abs(cos(mRunningTime * 2.0f)) + 1.0f;
	//mDuckRenderObject.transform.position.y = height;
}

void GameState::Render()
{
	mShadowEffect.Begin();
	mShadowEffect.Render(mDuckRenderObject);
	mShadowEffect.End();

	mStandardEffect.Begin();
	mStandardEffect.Render(mDuckRenderObject);
	mStandardEffect.Render(mGroundRenderObject);
	mStandardEffect.End();

	//SimpleDraw::AddTransform(Math::Matrix4::Identity);
	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f %s", mFPS, mPause ? "(Paused!)" : "");
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}
		
		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}

	if (ImGui::CollapsingHeader("Duck", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Ambient##Duck", &mDuckRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Duck", &mDuckRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Duck", &mDuckRenderObject.material.specular.r);
		ImGui::DragFloat("Power##Duck", &mDuckRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Ambient##Ground", &mGroundRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Ground", &mGroundRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Ground", &mGroundRenderObject.material.specular.r);
		ImGui::DragFloat("Power##Ground", &mGroundRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	ImGui::End();
}