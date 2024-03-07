#include "GameState.h"

#include "GameManagerService.h"
#include "TileMapService.h"

#include "PlayerAnimatorComponent.h"
#include "PlayerControllerComponent.h"

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
		return false;
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<GameManagerService>();
	TileMapService* tileMapService = mGameWorld.AddService<TileMapService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/pacman_level.json");

	tileMapService->LoadTiles("tiles.txt");
	tileMapService->LoadTileMap("map.txt");
	tileMapService->LoadFlipMap("flipmap.txt");
	tileMapService->LoadPivotMap("pivotmap.txt");

	mPlayerAnimatorComponent = mGameWorld.FindGameObject("PacMan")->GetComponent<PlayerAnimatorComponent>();
}

void GameState::Terminate()
{
	mGameWorld.Terminate();
	mPlayerAnimatorComponent = nullptr;
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();
	mPlayerAnimatorComponent->Render();
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();
}
