#include "GameManagerService.h"
#include "TileMapService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Input;

void GameManagerService::Initialize()
{
	SetServiceName("Game Manager Service");

	mTileMapService = GetWorld().GetService<TileMapService>();
}

void GameManagerService::Terminate()
{
	mTileMapService = nullptr;
}

void GameManagerService::Update(float deltaTime)
{
}

void GameManagerService::Render()
{
}

void GameManagerService::DebugUI()
{
	ImGui::DragInt("Level##GameManagerService", &mLevel, 0.5f);
	ImGui::DragInt("Lives##GameManagerService", &mPlayerLives, 0.5f);
	ImGui::DragInt("Points##GameManagerService", &mPlayerPoints, 0.5f);
	ImGui::DragInt("Remaining Pellet Count##GameManagerService", &mRemainingPelletCount, 0.5f);
}


void GameManagerService::AtePellet(PelletType pelletType)
{
	mPlayerPoints += static_cast<int>(pelletType);
	if (--mRemainingPelletCount <= 0)
	{
		SetupLevel(mLevel++);
	}
}

void GameManagerService::SetupLevel(int level)
{
	// Restart Map
	// Restart Ghost
	// Restart Player
	// Adjust stats according to each level
	mRemainingPelletCount = mMaxPelletCount;

	mTileMapService->ReloadMap();
}

void GameManagerService::RestartLevel()
{
	// Set ghost back to box
	// Set player to start position
	mRemainingPelletCount = mMaxPelletCount;

	mTileMapService->ReloadMap();
}

void GameManagerService::RestartGame()
{
	// Restart Map
	// Restart Ghost
	// Restart Player

	mLevel = 1;
	mPlayerPoints = 0;
	mPlayerLives = mPlayerStartingLives;
	mRemainingPelletCount = mMaxPelletCount;

	SetupLevel(mLevel);
}
