#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({0.0f, 3.0f, -12.0f});
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
	mPostProccessingEffect.SetMode(PostProcessingEffect::Mode::None);
	mPostProccessingEffect.SetTexture(&mBaseRenderTarget, 0);
	mPostProccessingEffect.SetTexture(&mGaussianBlurEffect.GetResultTexture(), 1);
	mPostProccessingEffect.SetIntensity(25.0f);

	mGaussianBlurEffect.Initialize();
	mGaussianBlurEffect.SetSourceTexture(mBloomRenderTarget);
	mGaussianBlurEffect.BlurIterations() = 10.0f;
	mGaussianBlurEffect.BlurSaturation() = 1.0f;

	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();
	mBaseRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);
	mBloomRenderTarget.Initialize(screenWidth, screenHeight, RenderTarget::Format::RGBA_U8);

	auto tm = TextureManager::Get();
	// Sun
	mSunRenderObject.material.emissive = Colors::White;
	mSunRenderObject.material.power = 10.0f;
	mSunRenderObject.diffuseMapId = tm->LoadTexture("sun.jpg");
	mSunRenderObject.meshBuffer.Initialize(MeshBuilder::CreateSphere(512, 1024, 5.0f));

	//Earth
	mEarthRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mEarthRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mEarthRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mEarthRenderObject.material.power = 10.0f;
	mEarthRenderObject.diffuseMapId = tm->LoadTexture("earth.jpg");
	mEarthRenderObject.specularMapId = tm->LoadTexture("earth_spec.jpg");
	mEarthRenderObject.bumpMapId = tm->LoadTexture("earth_bump.jpg");
	mEarthRenderObject.normalMapId = tm->LoadTexture("earth_normal.jpg");
	mEarthRenderObject.meshBuffer.Initialize(MeshBuilder::CreateSphere(512, 1024, 1.0f));

	mScreenQuad.meshBuffer.Initialize(MeshBuilder::CreateScreenQuad());
}

void GameState::Terminate()
{
	mEarthRenderObject.Terminate();
	mSunRenderObject.Terminate();

	mBloomRenderTarget.Terminate();
	mBaseRenderTarget.Terminate();

	mGaussianBlurEffect.Terminate();
	mPostProccessingEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 1.0f, 10.0f, 0.1f, deltaTime);
	mFPS = 1.0f / deltaTime;

	// Update Earth Transform
	Vector3 earthPosition = { 0.0f, 0.0f, -mEarthOffSet };
	mEarthRenderObject.transform.position = TransformCoord(earthPosition, Matrix4::RotationY(mEarthRevolution));
	mEarthRenderObject.transform.rotation.y = mEarthRotation;
}

void GameState::Render()
{
	mBaseRenderTarget.BeginRender();
	{
		mStandardEffect.Begin();
		mStandardEffect.Render(mSunRenderObject);
		mStandardEffect.Render(mEarthRenderObject);
		mStandardEffect.End();

		SimpleDraw::AddPlane(25, Colors::White);
		SimpleDraw::AddTransform(Math::Matrix4::Identity);
		SimpleDraw::Render(mCamera);
	}
	mBaseRenderTarget.EndRender();

	mBloomRenderTarget.BeginRender();
	{
		//Swap in blank material
		Material dummyMaterial;
		dummyMaterial.power = 1.0f;
		std::swap(mEarthRenderObject.material, dummyMaterial);

		mStandardEffect.Begin();
		mStandardEffect.Render(mSunRenderObject);
		mStandardEffect.Render(mEarthRenderObject);
		mStandardEffect.End();

		// Restore original material
		std::swap(mEarthRenderObject.material, dummyMaterial);
	}
	mBloomRenderTarget.EndRender();

	mGaussianBlurEffect.Begin();
	{
		mGaussianBlurEffect.Render(mScreenQuad);
	}
	mGaussianBlurEffect.End();

	mPostProccessingEffect.Begin();
	{
		mPostProccessingEffect.Render(mScreenQuad);
	}
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

		ImGui::DragFloat("OffSet##Earth", &mEarthOffSet, 0.01f);
		ImGui::DragFloat("Rotation##Earth", &mEarthRotation, 0.01f);
		ImGui::DragFloat("Revolution##Earth", &mEarthRevolution, 0.01f);
	}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mPostProccessingEffect.DebugUI();
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Blur Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragInt("Blur Iteration", &mGaussianBlurEffect.BlurIterations(), 1, 1, 100);
		ImGui::DragFloat("Blur Saturation", &mGaussianBlurEffect.BlurSaturation(), 0.001f, 1.0f, 100.0f);
	}

	ImGui::Separator();

	ImGui::End();

	ImGui::Begin("Render Targets", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Base");
	ImGui::Image(mBaseRenderTarget.GetRawData(), {256, 144});
	ImGui::Text("Bloom");
	ImGui::Image(mBloomRenderTarget.GetRawData(), { 256, 144 });
	ImGui::Text("Horizontal Blur");
	ImGui::Image(mGaussianBlurEffect.GetHorizontalBlurTexture().GetRawData(), { 256, 144 });
	ImGui::Text("Vertical Blur");
	ImGui::Image(mGaussianBlurEffect.GetVerticalBlurTexture().GetRawData(), { 256, 144 });
	ImGui::End();
}