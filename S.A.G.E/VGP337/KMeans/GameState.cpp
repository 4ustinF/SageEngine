#include "GameState.h"
#include <ImGui/Inc/ImPlot.h>

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::ML;

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

	mUnitTexture.Initialize(L"../../Assets/Sprites/X/scv_01.png");

	for (size_t i = 0; i < 50; ++i)
	{
		const float x = Math::Random::UniformFloat(0.0f, 800.0f);
		const float y = Math::Random::UniformFloat(0.0f, 600.0f);
		mUnits.emplace_back(Unit{ {x, y} });
	}
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	//mGameWorld.Update(deltaTime);

	const auto& inputSystem = InputSystem::Get();
	if (inputSystem->IsKeyPressed(KeyCode::SPACE))
	{
		mAppLog.clear();
	}
}

void GameState::Render()
{
	//mGameWorld.Render();
}

void GameState::DebugUI()
{
	//mGameWorld.DebugUI();

	ImGui::SetNextWindowSize({800.0f, 600.0f});
	ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoResize);

	for (const auto& unit : mUnits)
	{

	}

	ImGui::End();
}
