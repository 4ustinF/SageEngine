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
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/bare.json");
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);

	const auto& inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::SPACE))
	{
		mAppLog.AddLog("Bam!\n");
	}
}

void GameState::Render()
{
	mGameWorld.Render();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();

	mAppLog.Draw("Console");
}