#include "GameManagerService.h"
#include "TileMapService.h"

#include "PlayerControllerComponent.h"
#include "PlayerAnimatorComponent.h"
#include "GhostControllerComponent.h"
#include "GhostAnimatorComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Input;
using namespace SAGE::Coroutine;

void GameManagerService::Initialize()
{
	SetServiceName("Game Manager Service");

	mTileMapService = GetWorld().GetService<TileMapService>();
	mCoroutineSystem = CoroutineSystem::Get();

	// Init audio
	mSoundEffectManager = SoundEffectManager::Get();
	mGhostEatenSoundID = mSoundEffectManager->Load("eat_ghost.wav");
	//mMunchID = mSoundEffectManager->Load("munch.wav"); // Need a better soundFX

	auto tm = TextureManager::Get();
	mPathFindingTextureID = tm->LoadTexture("../Sprites/PacMan/pathfinding.png");
	mEatenPointsTextureIDs[0] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_200.png");
	mEatenPointsTextureIDs[1] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_400.png");
	mEatenPointsTextureIDs[2] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_800.png");
	mEatenPointsTextureIDs[3] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_1600.png");

	spriteRenderer = SpriteRenderer::Get();

	SetLevelData();
	SetIntersectionPoints();
}

void GameManagerService::Terminate()
{
	// Audio
	mMunchID = 0;
	mGhostEatenSoundID = 0;
	mSoundEffectManager = nullptr;

	// Pellets
	mCachedSmallPelletCords.clear();
	mCachedBigPelletCords.clear();

	// Ghost
	mBlinkyController = nullptr;
	mBlinkyAnimator = nullptr;

	// Player
	mPlayerAnimator = nullptr;
	mPlayerController = nullptr;

	// Other
	mLevels.clear();
	mIntersections.clear();
	mIntersectionsPositions.clear();
	mTileMapService = nullptr;
	mPathFindingTextureID = 0;
	mCoroutineSystem = nullptr;
	spriteRenderer = nullptr;
}

void GameManagerService::Update(float deltaTime)
{
	if (mIsInFrenzy)
	{
		CheckIfPlayerAteGhost();
		mFrightTimer -= deltaTime;
		if (mFrightTimer <= 0.0f)
		{
			mPlayerController->SetPlayerSpeed(mCurrentLevelData.PacManSpeed);
			mIsInFrenzy = false;
			mGhostAteInFrenzy = 0;

			if (mBlinkyController->GetGhostMode() == GhostMode::Frightened) // TODO: Only set ghost back if they were not eaten. If they were eaten they will sort themselves out.
			{
				SetGhostChaseScatterMode(mPrevGhostMode);
			}
		}
	}
	else
	{
		if (mTickScatterChaseTimer)
		{
			mScatterChaseTimer -= deltaTime;

			if (mScatterChaseTimer <= 0.0f)
			{
				if (++mScatterChaseIndex < static_cast<int>(mScatterChaseTimes.size())) {
					mScatterChaseTimer += mScatterChaseTimes[mScatterChaseIndex];
					SetGhostChaseScatterMode(mGhostMode == GhostMode::Scatter ? GhostMode::Chase : GhostMode::Scatter);
				}
				else {
					mTickScatterChaseTimer = false;
					SetGhostChaseScatterMode(GhostMode::Chase); // Chase Indefinitely
				}
			}
		}

		CheckIfGhostAtePlayer();
	}

	// Timer to display points
	if (mDisplayEatenPointsTimer > 0.0f) {
		mDisplayEatenPointsTimer -= deltaTime;
	}
}

void GameManagerService::Render()
{
	mPlayerAnimator->Render();
	mBlinkyAnimator->Render();

	if (mDisplayEatenPointsTimer > 0.0f)
	{
		spriteRenderer->Draw(mEatenPointsTextureIDs[mTextureIDIndex], mGhostEatenPosition, 0.0, Pivot::Center, Flip::None);
	}

	//// TODO: This is for debugging purposes
	//Vector2 offset = mTileMapService->GetWorldOffset();

	//for (const auto& pos : mBlinkyController->mTargetNodePositions)
	//{
	//	SpriteRenderer::Get()->Draw(mPathFindingTextureID, pos + offset, 0.0, Pivot::Center, Flip::None);
	//}

	//for (const auto& pos : mIntersectionsPositions)
	//{
	//	SpriteRenderer::Get()->Draw(mPathFindingTextureID, pos + offset, 0.0, Pivot::Center, Flip::None);
	//}
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

	//Player
	auto& world = GetWorld();
	GameObject* playerObject = world.FindGameObject("PacMan");
	mPlayerController = playerObject->GetComponent<PlayerControllerComponent>();
	mPlayerAnimator = playerObject->GetComponent<PlayerAnimatorComponent>();

	// Ghost
	GameObject* blinkyObject = world.FindGameObject("Blinky");
	mBlinkyController = blinkyObject->GetComponent<GhostControllerComponent>();
	mBlinkyAnimator = blinkyObject->GetComponent<GhostAnimatorComponent>();

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
	//mSoundEffectManager->Play(mMunchID, false, 0.5f, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));

	mPlayerPoints += static_cast<int>(pelletType);

	if (pelletType == PelletType::Big) {
		mPlayerController->SetPlayerSpeed(mCurrentLevelData.FrightPacManSpeed);
		mIsInFrenzy = true;
		mFrightTimer = mCurrentLevelData.FrightTime;
		mPrevGhostMode = mGhostMode;
		SetGhostChaseScatterMode(GhostMode::Frightened);
	}

	if (--mRemainingPelletCount <= 0) {
		mCoroutineSystem->StartCoroutine(GoToNextLevel());
	}
}


bool GameManagerService::IsIntersectionPoint(Vector2Int pointToCheck) const
{
	for (const Vector2Int& intersection : mIntersections)
	{
		if (pointToCheck == intersection)
		{
			return true;
		}
	}
	return false;
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

	mCurrentLevelData = mLevels[Min(mLevel, static_cast<int>(mLevels.size()) - 1) - 1];

	mPlayerController->Respawn();
	mPlayerController->SetPlayerSpeed(mCurrentLevelData.PacManSpeed);
	mBlinkyController->Respawn();

	if (mLevel > 1) { // Don't need to populate the pellets on the first level as that is preset for us there.
		RepopulatePellets();
	}

	// Scatter Chase Routine
	mScatterChaseIndex = 0;
	mScatterChaseTimer = mScatterChaseTimes[mScatterChaseIndex];
	mTickScatterChaseTimer = true;
	SetGhostChaseScatterMode(GhostMode::Scatter);

	mIsInFrenzy = false;
}

void GameManagerService::RestartLevel()
{
	mPlayerController->Respawn();
	mBlinkyController->Respawn();

	// Scatter Chase Routine
	mScatterChaseIndex = 0;
	mScatterChaseTimer = mScatterChaseTimes[mScatterChaseIndex];
	mTickScatterChaseTimer = true;
	SetGhostChaseScatterMode(GhostMode::Scatter);

	mIsInFrenzy = false;
}

void GameManagerService::SetLevelData()
{
	mLevels.reserve(21);
	mLevels.push_back({ BonusSymbol::Cherries,	0.8f, 0.75f, 0.40f,  20, 0.8f, 10, 0.85f, 0.90f, 0.50f, 6.f, 5 });
	mLevels.push_back({ BonusSymbol::Strawberry,0.9f, 0.85f, 0.45f,  30, 0.9f, 15, 0.95f, 0.95f, 0.55f, 5.f, 5 });
	mLevels.push_back({ BonusSymbol::Peach,		0.9f, 0.85f, 0.45f,  40, 0.9f, 20, 0.95f, 0.95f, 0.55f, 4.f, 5 });
	mLevels.push_back({ BonusSymbol::Peach,		0.9f, 0.85f, 0.45f,  40, 0.9f, 20, 0.95f, 0.95f, 0.55f, 3.f, 5 });
	mLevels.push_back({ BonusSymbol::Apple,		1.0f, 0.95f, 0.50f,  40, 1.0f, 20, 1.05f, 1.00f, 0.60f, 2.f, 5 });
	mLevels.push_back({ BonusSymbol::Apple,		1.0f, 0.95f, 0.50f,  50, 1.0f, 25, 1.05f, 1.00f, 0.60f, 5.f, 5 });
	mLevels.push_back({ BonusSymbol::Grapes,	1.0f, 0.95f, 0.50f,  50, 1.0f, 25, 1.05f, 1.00f, 0.60f, 2.f, 5 });
	mLevels.push_back({ BonusSymbol::Grapes,	1.0f, 0.95f, 0.50f,  50, 1.0f, 25, 1.05f, 1.00f, 0.60f, 2.f, 5 });
	mLevels.push_back({ BonusSymbol::Galaxian,	1.0f, 0.95f, 0.50f,  60, 1.0f, 30, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Galaxian,	1.0f, 0.95f, 0.50f,  60, 1.0f, 30, 1.05f, 1.00f, 0.60f, 5.f, 5 });
	mLevels.push_back({ BonusSymbol::Bell,		1.0f, 0.95f, 0.50f,  60, 1.0f, 30, 1.05f, 1.00f, 0.60f, 2.f, 5 });
	mLevels.push_back({ BonusSymbol::Bell,		1.0f, 0.95f, 0.50f,  80, 1.0f, 40, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f,  80, 1.0f, 40, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f,  80, 1.0f, 40, 1.05f, 1.00f, 0.60f, 3.f, 5 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f, 100, 1.0f, 50, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f, 100, 1.0f, 50, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f, 100, 1.0f, 50, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f, 100, 1.0f, 50, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f, 120, 1.0f, 60, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		1.0f, 0.95f, 0.50f, 120, 1.0f, 60, 1.05f, 1.00f, 0.60f, 1.f, 3 });
	mLevels.push_back({ BonusSymbol::Key,		0.9f, 0.95f, 0.50f, 120, 1.0f, 60, 1.05f, 1.00f, 0.60f, 1.f, 3 });
}

void GameManagerService::SetIntersectionPoints()
{
	mIntersections.reserve(34);

	mIntersections.push_back(Vector2Int(8, 2));
	mIntersections.push_back(Vector2Int(23, 2));
	mIntersections.push_back(Vector2Int(3, 6));
	mIntersections.push_back(Vector2Int(8, 6));
	mIntersections.push_back(Vector2Int(11, 6));
	mIntersections.push_back(Vector2Int(14, 6));
	mIntersections.push_back(Vector2Int(17, 6));
	mIntersections.push_back(Vector2Int(20, 6));
	mIntersections.push_back(Vector2Int(23, 6));
	mIntersections.push_back(Vector2Int(28, 6));
	mIntersections.push_back(Vector2Int(8, 9));
	mIntersections.push_back(Vector2Int(23, 9));
	mIntersections.push_back(Vector2Int(14, 12));
	mIntersections.push_back(Vector2Int(17, 12));
	mIntersections.push_back(Vector2Int(8, 15));
	mIntersections.push_back(Vector2Int(11, 15));
	mIntersections.push_back(Vector2Int(20, 15));
	mIntersections.push_back(Vector2Int(23, 15));
	mIntersections.push_back(Vector2Int(11, 18));
	mIntersections.push_back(Vector2Int(20, 18));
	mIntersections.push_back(Vector2Int(8, 21));
	mIntersections.push_back(Vector2Int(11, 21));
	mIntersections.push_back(Vector2Int(20, 21));
	mIntersections.push_back(Vector2Int(23, 21));
	mIntersections.push_back(Vector2Int(8, 24));
	mIntersections.push_back(Vector2Int(11, 24));
	mIntersections.push_back(Vector2Int(14, 24));
	mIntersections.push_back(Vector2Int(17, 24));
	mIntersections.push_back(Vector2Int(20, 24));
	mIntersections.push_back(Vector2Int(23, 24));
	mIntersections.push_back(Vector2Int(5, 27));
	mIntersections.push_back(Vector2Int(26, 27));
	mIntersections.push_back(Vector2Int(14, 30));
	mIntersections.push_back(Vector2Int(17, 30));

	mIntersectionsPositions.reserve(mIntersections.size());
	for (const auto& cord : mIntersections)
	{
		// TODO: Tile size and half tile size should be cached
		mIntersectionsPositions.push_back({cord.x * 24.0f + 12.0f, cord.y * 24.0f + 12.0f});
	}
}

void GameManagerService::AteGhost()
{
}

void GameManagerService::CheckIfGhostAtePlayer()
{
	if (mBlinkyController->GetGhostMode() == GhostMode::Eaten || mPlayerController->GetIsPlayerInvincible())
	{
		return;
	}

	if (mBlinkyController->GetTileCords() == mPlayerController->GetPlayerCords())
	{
		if (--mPlayerLives > 0) {
			RestartLevel();
			return;
		}

		RestartGame();
	}
}

void GameManagerService::CheckIfPlayerAteGhost()
{
	if (mBlinkyController->GetGhostMode() == GhostMode::Eaten)
	{
		return;
	}

	if (mPlayerController->GetPlayerCords() == mBlinkyController->GetTileCords())
	{
		mSoundEffectManager->Play(mGhostEatenSoundID, false, 0.5f, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));
		mDisplayEatenPointsTimer = mDisplayEatenPointsMaxTimer;
		mGhostEatenPosition = mBlinkyController->GetPosition();
		mTextureIDIndex = mGhostAteInFrenzy;

		mPlayerPoints += mPointsPerGhostAte * static_cast<int>(std::pow(2, mGhostAteInFrenzy++));
		mBlinkyController->IsAten();
	}
}

void GameManagerService::SetGhostChaseScatterMode(GhostMode mode)
{
	mGhostMode = mode;
	mBlinkyController->SetGhostMode(mGhostMode);
}


Enumerator GameManagerService::GoToNextLevel()
{
	return [=](CoroPush& yield_return)
		{
			// Prevent ghost from killing player here?
			yield_return(new WaitForSeconds(0.15f));
			++mLevel;
			SetupLevel();
		};
}
