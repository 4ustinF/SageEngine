#include "GameManagerService.h"
#include "TileMapService.h"

#include "PlayerControllerComponent.h"

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
	mCachedSmallPelletCords.clear();
	mCachedBigPelletCords.clear();

	mPlayerController = nullptr;
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


void GameManagerService::SetupGame()
{
	// Load Map
	mTileMapService->LoadTiles("tiles.txt");
	mTileMapService->LoadTileMap("map.txt");
	mTileMapService->LoadFlipMap("flipmap.txt");
	mTileMapService->LoadPivotMap("pivotmap.txt");

	mLevel = 1;
	mPlayerPoints = 0;
	mPlayerLives = mPlayerStartingLives;
	mRemainingPelletCount = mMaxPelletCount;

	CachePelletLocations();
}

void GameManagerService::StartGame()
{
	SetupLevel();
}

void GameManagerService::AtePellet(PelletType pelletType)
{
	mPlayerPoints += static_cast<int>(pelletType);
	if (--mRemainingPelletCount <= 0)
	{
		++mLevel;
		SetupLevel();
	}
}

void GameManagerService::CachePelletLocations()
{
	const int columns = mTileMapService->GetColumns();
	const int rows = mTileMapService->GetRows();
	mCachedSmallPelletCords.reserve(240);	// Number of small pellets in the game
	mCachedBigPelletCords.reserve(4);		// Number of big pellets in the game

	for (int x = 0 ; x < columns; ++x)
	{
		for (int y = 0; y < rows; ++y)
		{
			const int tileIndex = mTileMapService->GetTile(x, y).tileIndex;
			if (tileIndex == 1) { 
				mCachedSmallPelletCords.push_back({x, y});
			}
			else if (tileIndex == 2) {
				mCachedBigPelletCords.push_back({ x, y });
			}
		}
	}
}

void GameManagerService::RepopulatePellets()
{
	for (const Vector2Int& tileCord : mCachedSmallPelletCords) {
		mTileMapService->GetTile(tileCord).tileIndex = 1;
	}

	for (const Vector2Int& tileCord : mCachedBigPelletCords) {
		mTileMapService->GetTile(tileCord).tileIndex = 2;
	}

	mRemainingPelletCount = mMaxPelletCount;
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

	RepopulatePellets();
	SetupLevel();
}

void GameManagerService::SetupLevel()
{
	// Restart Map
	// Restart Ghost
	// Restart Player
	// Adjust stats according to each level

	mPlayerController->Respawn();

	if (mLevel > 1) { // Don't need to populate the pellets on the first level as that is preset for us there.
		RepopulatePellets();
	}
}

void GameManagerService::RestartLevel()
{
	// Set ghost back to box
	// Set player to start position

	mPlayerController->Respawn();
}
