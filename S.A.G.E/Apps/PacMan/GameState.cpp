#include "GameState.h"

#include "GameManagerService.h"
#include "TileMapService.h"

#include "PlayerAnimatorComponent.h"
#include "PlayerControllerComponent.h"
#include "GhostControllerComponent.h"
#include "GhostAnimatorComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::ML;

namespace
{
	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		if (strcmp(componentName, "PlayerControllerComponent") == 0)
		{
			auto playerControllerComponent = gameObject.AddComponent<PlayerControllerComponent>();
			return true;
		}
		else if (strcmp(componentName, "PlayerAnimatorComponent") == 0)
		{
			auto playerAnimatorComponent = gameObject.AddComponent<PlayerAnimatorComponent>();
			return true;
		}
		else if (strcmp(componentName, "GhostControllerComponent") == 0)
		{
			auto ghostControllerComponent = gameObject.AddComponent<GhostControllerComponent>();
			return true;
		}
		else if (strcmp(componentName, "GhostAnimatorComponent") == 0)
		{
			auto ghostAnimatorComponent = gameObject.AddComponent<GhostAnimatorComponent>();
			if (value.HasMember("MovementSprites")) {
				const auto movementSprites = value["MovementSprites"].GetArray();
				std::array<std::filesystem::path, 6> spriteFilePaths;
				for (int i = 0; i < 6; ++i) {
					spriteFilePaths[i] = movementSprites[i].GetString();
				}
				ghostAnimatorComponent->InitGhostSprites(spriteFilePaths);
			}
			return true;
		}
		return false;
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<TileMapService>();
	GameManagerService* gameManagerService = mGameWorld.AddService<GameManagerService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/pacman_level.json");

	gameManagerService->SetupGame();
	gameManagerService->StartGame();

	mInputSystem = InputSystem::Get();
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	if (mInputSystem->IsKeyPressed(KeyCode::SPACE)) {
		mIsPaused = !mIsPaused;
	}

	if (!mIsPaused) {
		mGameWorld.Update(deltaTime);
	}
}

void GameState::Render()
{
	mGameWorld.Render();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();
}
