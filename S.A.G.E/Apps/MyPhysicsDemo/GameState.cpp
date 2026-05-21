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
	mGameWorld.Initialize(100);

	GameObjectFactory::SetMakeOverride(OnMake);
	//mGameWorld.LoadLevel("../../Assets/Level/your_level.json");
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();

	ImGui::Begin("Debug Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::End();
}
