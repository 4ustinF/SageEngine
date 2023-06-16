#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

void GameState::Initialize()
{
	auto inputSystem = InputSystem::Get();
	//inputSystem->isSnap = true;

	auto tm = TextureManager::Get();

	const float turnTime = 1.0f;
	mAnimation = AnimationBuilder()
		.AddPositionKey({0.0f, 3.0f, 0.0f}, 0.0f)
		.AddPositionKey({8.0f, 3.0f, 0.0f}, 2.0f)
		.AddPositionKey({0.0f, 3.0f, 0.0f}, 4.0f)
		.AddPositionKey({-8.0f, 3.0f, 0.0f}, 6.0f)
		.AddPositionKey({0.0f, 3.0f, 0.0f}, 8.0f)
		.AddRotationKey(Quaternion::RotationEuler({0.0f, Constants::HalfPi, 0.0f}), 2.0f - turnTime)
		.AddRotationKey(Quaternion::RotationEuler({0.0f,-Constants::HalfPi, 0.0f}), 2.0f + turnTime)
		.AddRotationKey(Quaternion::RotationEuler({0.0f,-Constants::HalfPi, 0.0f}), 6.0f - turnTime)
		.AddRotationKey(Quaternion::RotationEuler({0.0f, Constants::HalfPi, 0.0f}), 6.0f + turnTime)
		.AddScaleKey(Vector3::One, 0.0f)
		.AddScaleKey({2.0f, 2.0f, 2.0f }, 4.0f)
		.AddScaleKey(Vector3::One, 8.0f)
		.Get();

	mCamera.SetPosition({0.0f, 3.0f, -10.0f});
	mCamera.SetLookAt({0.0f, 0.0f, 0.0f});

	mDirectionalLight.direction = Math::Normalize({1.0f, -1.0f, 1.0f});
	mDirectionalLight.ambient = { 0.6f, 0.6f, 0.6f, 1.0f};
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f};
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000023f);
	mStandardEffect.SetBumpWeight(0.25f);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);

	auto gs = GraphicsSystem::Get();
	const auto screenWidth = gs->GetBackBufferWidth();
	const auto screenHeight = gs->GetBackBufferHeight();

	// Ball
	mBallRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mBallRenderObject.material.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	mBallRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mBallRenderObject.material.power = 10.0f;
	mBallRenderObject.diffuseMapId = tm->LoadTexture("earth.jpg");
	mBallRenderObject.specularMapId = tm->LoadTexture("earth_spec.jpg");
	mBallRenderObject.bumpMapId = tm->LoadTexture("earth_bump.jpg");
	mBallRenderObject.normalMapId = tm->LoadTexture("earth_normal.jpg");
	mBallRenderObject.meshBuffer.Initialize(MeshBuilder::CreateSphere(512, 1024, 0.5f));
	mBallRenderObject.transform.position = { 0.0f, 3.0f, 0.0f };

	// Ground
	mGroundRenderObject.material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mGroundRenderObject.material.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	mGroundRenderObject.material.power = 10.0f;
	mGroundRenderObject.diffuseMapId = tm->LoadTexture("concrete.jpg");
	mGroundRenderObject.meshBuffer.Initialize(MeshBuilder::CreatePlane(25, 25, 1.0f));	
}

void GameState::Terminate()
{
	mGroundRenderObject.Terminate();
	mBallRenderObject.Terminate();
	mShadowEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	SAGE::CameraUtil::UpdateFPSCamera(mCamera, 1.0f, 10.0f, 0.1f, deltaTime);
	mFPS = 1.0f / deltaTime;

	//const float time = SAGE::Core::TimeUtil::GetTime();
	mAnimationTime += deltaTime * 0.5f;
	/*mFPS = mBallRenderObject.transform.position.x;*/
	const float duration = mAnimation.GetDuration();
	if (mAnimationTime > duration) {
		mAnimationTime -= duration;
	}

	//const Quaternion fix = Quaternion::RotationEuler({Constants::HalfPi, Constants::Pi, 0.0f});
	//mBallRenderObject.transform.position = mAnimation.GetPosition(mAnimationTime);
	//mBallRenderObject.transform.position.y += sin(time) * 0.1f;
	//mBallRenderObject.transform.rotation = fix * mAnimation.GetRotation(mAnimationTime);

	mBallRenderObject.transform.position = mAnimation.GetPosition(mAnimationTime);
	mBallRenderObject.transform.rotation = mAnimation.GetRotation(mAnimationTime);
	mBallRenderObject.transform.scale = mAnimation.GetScale(mAnimationTime);
}

void GameState::Render()
{
	mShadowEffect.Begin();
	mShadowEffect.Render(mBallRenderObject);
	mShadowEffect.End();

	mStandardEffect.Begin();
	mStandardEffect.Render(mGroundRenderObject);
	mStandardEffect.Render(mBallRenderObject);
	mStandardEffect.End();

	//SimpleDraw::AddTransform(Math::Matrix4::Identity);
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

	if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Ground", &mGroundRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Ground", &mGroundRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Ground", &mGroundRenderObject.material.specular.r);
		ImGui::ColorEdit4("Emissive##Ground", &mGroundRenderObject.material.emissive.r);
		ImGui::DragFloat("Power##Ground", &mGroundRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	if (ImGui::CollapsingHeader("Ball", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::ColorEdit4("Ambient##Ball", &mBallRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse##Ball", &mBallRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular##Ball", &mBallRenderObject.material.specular.r);
		ImGui::ColorEdit4("Emissive##Ball", &mBallRenderObject.material.emissive.r);
		ImGui::DragFloat("Power##Ball", &mBallRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	ImGui::End();
}