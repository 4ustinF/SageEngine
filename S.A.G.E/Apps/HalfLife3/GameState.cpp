#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

namespace
{
	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		return false;
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>()->SetSampleFilter(Sampler::Filter::Point);
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/HalfLife/halflife_level.json");
	mGameWorld.CreateGameObject("../../Assets/Templates/empty.json");

	auto tm = TextureManager::Get();

	// Directional Light
	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.6f, 0.6f, 0.6f, 1.0f };
	mDirectionalLight.diffuse = { 0.7f, 0.7f, 0.7f, 1.0f };
	mDirectionalLight.specular = { 0.7f, 0.7f, 0.7f, 1.0f };

	// Standard Effect
	mStandardEffect.Initialize();
	mStandardEffect.SetCamera(mGameWorld.GetService<CameraService>()->GetCamera());
	mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetShadowMap(&mShadowEffect.GetDepthMap());
	mStandardEffect.SetDepthBias(0.000023f);
	mStandardEffect.SetBumpWeight(0.25f);

	mShadowEffect.Initialize();
	mShadowEffect.SetDirectionalLight(mDirectionalLight);
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
	mShadowEffect.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();
	SimpleDraw::Render(mGameWorld.GetService<CameraService>()->GetCamera());

	mStandardEffect.Begin();
	mStandardEffect.End();

	mStandardEffect.Begin();
	mStandardEffect.End();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();

	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::DragFloat3("Direction##Light", &mDirectionalLight.direction.x, 0.01f, -1.0f, 1.0f)) {
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient##Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse##Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular##Light", &mDirectionalLight.specular.r);
	}

	//if (ImGui::CollapsingHeader("Wall", ImGuiTreeNodeFlags_CollapsingHeader))
	//{
	//	ImGui::ColorEdit4("Ambient##Wall", &mWallRenderObject.material.ambient.r);
	//	ImGui::ColorEdit4("Diffuse##Wall", &mWallRenderObject.material.diffuse.r);
	//	ImGui::ColorEdit4("Specular##Wall", &mWallRenderObject.material.specular.r);
	//	ImGui::ColorEdit4("Emissive##Wall", &mWallRenderObject.material.emissive.r);
	//	ImGui::DragFloat("Power##Wall", &mWallRenderObject.material.power, 1.0f, 1.0f, 100.0f);
	//}

	// Includes
	ImGui::Separator();
	mStandardEffect.DebugUI();
	ImGui::Separator();
	mShadowEffect.DebugUI();
	ImGui::Separator();
	ImGui::End();
}
