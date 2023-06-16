#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({0.0f, 3.0f, -3.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	mDirectionalLight.direction = Math::Normalize({1.0f, -1.0f, 1.0f});
	mDirectionalLight.ambient = { 0.0f, 0.0f, 0.0f, 1.0f};
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f};
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetBumpWeight(0.25f);

	mPostProccessingEffect.Initialize();
	mPostProccessingEffect.SetTexture(&mRenderTarget);
	mPostProccessingEffect.SetMode(PostProcessingEffect::Mode::None);
	mPostProccessingEffect.SetIntensity(25.0f);

	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();
	mRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);

	mEarthRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mEarthRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mEarthRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mEarthRenderObject.material.power = 10.0f;

	auto tm = TextureManager::Get();
	mEarthRenderObject.diffuseMapId = tm->LoadTexture("earth.jpg");
	mEarthRenderObject.specularMapId = tm->LoadTexture("earth_spec.jpg");
	mEarthRenderObject.bumpMapId = tm->LoadTexture("earth_bump.jpg");
	mEarthRenderObject.normalMapId = tm->LoadTexture("earth_normal.jpg");

	mMesh = MeshBuilder::CreateSphere(512, 1024, 1.0f);
	mEarthRenderObject.meshBuffer.Initialize(mMesh);

	mScreenQuad.meshBuffer.Initialize(MeshBuilder::CreateScreenQuad());
}

void GameState::Terminate()
{
	mEarthRenderObject.Terminate();

	mRenderTarget.Terminate();

	mPostProccessingEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 1.0f, 10.0f, 0.1f, deltaTime);

	mFPS = 1.0f / deltaTime;
}

void GameState::Render()
{
	mRenderTarget.BeginRender();
	{
		mStandardEffect.Begin();
		mStandardEffect.Render(mEarthRenderObject);
		mStandardEffect.End();

		SimpleDraw::AddPlane(20, Colors::White);
		SimpleDraw::AddTransform(Math::Matrix4::Identity);

		//Show nomal lines
		if (mShowNormals) {
			for (const auto& v : mMesh.vertices) {
				SimpleDraw::AddLine(v.position, v.position + v.normal, Colors::Cyan);
			}
		}

		SimpleDraw::Render(mCamera);
	}
	mRenderTarget.EndRender();

	mPostProccessingEffect.Begin();
	mPostProccessingEffect.Render(mScreenQuad);
	mPostProccessingEffect.End();
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("FPS: %f", mFPS);
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

	if (ImGui::CollapsingHeader("Earth", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Ambient##Earth", &mEarthRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Earth", &mEarthRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Earth", &mEarthRenderObject.material.specular.r);
		ImGui::DragFloat("Power##Earth", &mEarthRenderObject.material.power, 1.0f, 1.0f, 100.0f);

		ImGui::DragFloat3("Position", &mEarthRenderObject.transform.position.x, 0.01f);
		ImGui::DragFloat3("Rotation", &mEarthRenderObject.transform.rotation.x, 0.01f);
	}
	ImGui::Checkbox("Show Normals", &mShowNormals);
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mPostProccessingEffect.DebugUI();
	ImGui::End();
}