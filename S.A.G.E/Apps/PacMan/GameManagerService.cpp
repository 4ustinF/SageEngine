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
	mBonusSymbolSoundID = mSoundEffectManager->Load("eat_bonus.wav");
	//mMunchID = mSoundEffectManager->Load("munch.wav"); // Need a better soundFX

	auto tm = TextureManager::Get();
	mPathFindingTextureID = tm->LoadTexture("../Sprites/PacMan/pathfinding.png");
	mEatenPointsTextureIDs[0] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_200.png");
	mEatenPointsTextureIDs[1] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_400.png");
	mEatenPointsTextureIDs[2] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_800.png");
	mEatenPointsTextureIDs[3] = tm->LoadTexture("../Sprites/PacMan/Ghost/Eaten/Points/sprite_1600.png");
	mPacmanLifeTextureID = tm->LoadTexture("../Sprites/PacMan/PacMan/Eat/sprite_eat_01.png");

	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Cherries, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_cherry.png")));
	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Strawberry, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_strawberry.png")));
	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Peach, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_peach.png")));
	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Apple, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_apple.png")));
	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Grapes, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_grapes.png")));
	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Galaxian, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_galaxian.png")));
	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Bell, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_bell.png")));
	mBonusSymbolTextureIDs.insert(std::make_pair(BonusSymbol::Key, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/sprite_key.png")));

	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Cherries, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_100.png")));
	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Strawberry, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_300.png")));
	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Peach, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_500.png")));
	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Apple, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_700.png")));
	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Grapes, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_1000.png")));
	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Galaxian, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_2000.png")));
	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Bell, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_3000.png")));
	mBonusSymbolsEatenPointsTextureIDsMap.insert(std::make_pair(BonusSymbol::Key, tm->LoadTexture("../Sprites/PacMan/BonusSymbols/Points/sprite_5000.png")));

	mSpriteRenderer = SpriteRenderer::Get();
	mFont = Font::Get();

	SetLevelData();
	SetIntersectionPoints();
}

void GameManagerService::Terminate()
{
	// Audio
	mMunchID = 0;
	mGhostEatenSoundID = 0;
	mBonusSymbolSoundID = 0;
	mSoundEffectManager = nullptr;

	// UI
	mFont = nullptr;
	mSpriteRenderer = nullptr;
	mPacmanLifeTextureID = 0;
	mPathFindingTextureID = 0;

	// Pellets
	mCachedSmallPelletCords.clear();
	mCachedBigPelletCords.clear();

	// Bonus Symbols
	mBonusSymbolTextureID = 0;
	mBonusSymbolTextureIDs.clear();

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
	mCoroutineSystem = nullptr;
	mBonusSymbolsEatenPointsTextureIDsMap.clear();
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

	// Timer to display ghost eaten points
	if (mDisplayEatenPointsTimer > 0.0f) {
		mDisplayEatenPointsTimer -= deltaTime;
	}

	// Timer to display symbol eaten points
	if (mDisplayBonusSymbolsEatenPointsTimer > 0.0f) {
		mDisplayBonusSymbolsEatenPointsTimer -= deltaTime;
	}

	if (mIsBonusSymbolActive)
	{
		mBonusSymbolTimer -= deltaTime;
		if (mBonusSymbolTimer <= 0.0f) { // Deactivate bonus symbol if it hasn't picked up in time 
			mIsBonusSymbolActive = false;
		}
		else {
			CheckIfPlayerAteBonusSymbol();
		}
	}
}

void GameManagerService::Render()
{
	if (mIsBonusSymbolActive)
	{
		mSpriteRenderer->Draw(mBonusSymbolTextureID, mBonusSymbolPosition);
	}

	mPlayerAnimator->Render();
	mBlinkyAnimator->Render();

	// Current Score
	mFont->Draw("Current Score", 12.0f, 9.0f, 25.0f, Colors::White);
	mFont->Draw(mPlayerPointsString.c_str(), 12.0f, 34.0f, 25.0f, Colors::White);

	// HighScore
	//float width = mFont->GetStringWidth(L"High Score", 25.0f); //672, 336, 136
	mFont->Draw("High Score", 268.0f, 9.0f, 25.0f, Colors::White); //268 = 336 - (136 / 2) = half screen width - half text size
	mFont->Draw(mHighScoreString.c_str(), mHighScoreStringXOffset, 34.0f, 25.0f, Colors::White);

	// Player Lives
	for (int i = 0; i < mPlayerLives; ++i) {
		mSpriteRenderer->Draw(mPacmanLifeTextureID, {50.0f + (i * 46.0f), 841.0f}, 0.0, Pivot::Center, Flip::Horizontal);
	}

	// Previous and current level bonus symbols
	for (int i = 0; i < mBonusSymbolTextureIds.size(); ++i)
	{
		mSpriteRenderer->Draw(mBonusSymbolTextureIds[i], { mBonusSymbolTextureIdsXStartOffset + (i * mBonusSymbolTextureIdsXSpacing), 841.0f});
	}

	// Points upon ghost eaten
	if (mDisplayEatenPointsTimer > 0.0f) {
		mSpriteRenderer->Draw(mEatenPointsTextureIDs[mTextureIDIndex], mGhostEatenPosition);
	}

	// Points upon bonus symbol eaten
	if (mDisplayBonusSymbolsEatenPointsTimer > 0.0f) {
		mSpriteRenderer->Draw(mBonusSymbolsEatenPointsTextureIDsMap[mCurrentLevelData.BonusSymbol], mBonusSymbolPosition);
	}

	//// TODO: This is for debugging purposes
	//const Vector2 offset = mTileMapService->GetWorldOffset();

	//for (const auto& pos : mBlinkyController->mTargetNodePositions)
	//{
	//	SpriteRenderer::Get()->Draw(mPathFindingTextureID, pos + offset);
	//}

	//for (const auto& pos : mIntersectionsPositions)
	//{
	//	SpriteRenderer::Get()->Draw(mPathFindingTextureID, pos + offset);
	//}
}

void GameManagerService::DebugUI()
{
	ImGui::DragInt("Level##GameManagerService", &mLevel, 0.5f);
	ImGui::DragInt("Lives##GameManagerService", &mPlayerLives, 0.5f);
	ImGui::DragInt("Points##GameManagerService", &mPlayerPoints, 0.5f);
	ImGui::DragInt("High Score##GameManagerService", &mMaxPlayerPoints, 0.5f);
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
	AddPlayerPoints(-mPlayerPoints);
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
	//PlayAudioOneShot(mMunchID);

	AddPlayerPoints(static_cast<int>(pelletType));

	if (pelletType == PelletType::Big) {
		mPlayerController->SetPlayerSpeed(mCurrentLevelData.FrightPacManSpeed);
		mIsInFrenzy = true;
		mFrightTimer = mCurrentLevelData.FrightTime;
		mPrevGhostMode = mGhostMode;
		SetGhostChaseScatterMode(GhostMode::Frightened);
	}

	--mRemainingPelletCount;

	if (mRemainingPelletCount == mBonusSymbol1RemainingPellets || mRemainingPelletCount == mBonusSymbol2RemainingPellets)
	{
		mIsBonusSymbolActive = true;
		mBonusSymbolTimer = mBonusSymbolMaxTime;
	}

	if (mRemainingPelletCount <= 0) {
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
	mLevel = 1;
	AddPlayerPoints(-mPlayerPoints);
	mPlayerLives = mPlayerStartingLives;
	mRemainingPelletCount = mMaxPelletCount;
	mPlayerPointsTillNextBonusLife = 0;

	mBonusSymbolTextureIds.clear();

	RepopulatePellets();
	SetupLevel();
}

void GameManagerService::SetupLevel()
{
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

	mIsBonusSymbolActive = false;
	mBonusSymbolTextureID = mBonusSymbolTextureIDs.at(mCurrentLevelData.BonusSymbol);

	mBonusSymbolTextureIds.push_back(mBonusSymbolTextureID);
	if (mBonusSymbolTextureIds.size() > 7) {
		mBonusSymbolTextureIds.erase(mBonusSymbolTextureIds.begin());
	}
	mBonusSymbolTextureIdsXStartOffset = mScreenWidth - (mBonusSymbolTextureIds.size() * mBonusSymbolTextureIdsXSpacing);

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

	mIntersections.push_back(Vector2Int(8, 1));
	mIntersections.push_back(Vector2Int(23, 1));
	mIntersections.push_back(Vector2Int(3, 5));
	mIntersections.push_back(Vector2Int(8, 5));
	mIntersections.push_back(Vector2Int(11, 5));
	mIntersections.push_back(Vector2Int(14, 5));
	mIntersections.push_back(Vector2Int(17, 5));
	mIntersections.push_back(Vector2Int(20, 5));
	mIntersections.push_back(Vector2Int(23, 5));
	mIntersections.push_back(Vector2Int(28, 5));
	mIntersections.push_back(Vector2Int(8, 8));
	mIntersections.push_back(Vector2Int(23, 8));
	mIntersections.push_back(Vector2Int(14, 11));
	mIntersections.push_back(Vector2Int(17, 11));
	mIntersections.push_back(Vector2Int(8, 14));
	mIntersections.push_back(Vector2Int(11, 14));
	mIntersections.push_back(Vector2Int(20, 14));
	mIntersections.push_back(Vector2Int(23, 14));
	mIntersections.push_back(Vector2Int(11, 17));
	mIntersections.push_back(Vector2Int(20, 17));
	mIntersections.push_back(Vector2Int(8, 20));
	mIntersections.push_back(Vector2Int(11, 20));
	mIntersections.push_back(Vector2Int(20, 20));
	mIntersections.push_back(Vector2Int(23, 20));
	mIntersections.push_back(Vector2Int(8, 23));
	mIntersections.push_back(Vector2Int(11, 23));
	mIntersections.push_back(Vector2Int(14, 23));
	mIntersections.push_back(Vector2Int(17, 23));
	mIntersections.push_back(Vector2Int(20, 23));
	mIntersections.push_back(Vector2Int(23, 23));
	mIntersections.push_back(Vector2Int(5, 26));
	mIntersections.push_back(Vector2Int(26, 26));
	mIntersections.push_back(Vector2Int(14, 29));
	mIntersections.push_back(Vector2Int(17, 29));

	mIntersectionsPositions.reserve(mIntersections.size());
	for (const auto& cord : mIntersections)
	{
		// TODO: Tile size and half tile size should be cached. Also take offset into consideration.
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
		PlayAudioOneShot(mGhostEatenSoundID);
		mDisplayEatenPointsTimer = mPointsMaxTime;
		mGhostEatenPosition = mBlinkyController->GetPosition();
		mTextureIDIndex = mGhostAteInFrenzy;

		AddPlayerPoints(mPointsPerGhostAte * static_cast<int>(std::pow(2, mGhostAteInFrenzy++)));
		mBlinkyController->IsAten();
	}
}

void GameManagerService::CheckIfPlayerAteBonusSymbol()
{
	if (IsRectOverlap(mPlayerController->GetRect(), mBonusSymbolRect))
	{
		mDisplayBonusSymbolsEatenPointsTimer = mPointsMaxTime;
		mIsBonusSymbolActive = false;
		PlayAudioOneShot(mBonusSymbolSoundID);
		AddPlayerPoints(static_cast<int>(mCurrentLevelData.BonusSymbol));
	}
}

void GameManagerService::AddPlayerPoints(int pointsToAdd)
{
	mPlayerPoints += pointsToAdd;
	mPlayerPointsString = std::to_string(mPlayerPoints);

	mPlayerPointsTillNextBonusLife += pointsToAdd;
	if (mPlayerPointsTillNextBonusLife >= mPlayerPointsNeededForBonusLife)
	{
		mPlayerPointsTillNextBonusLife -= mPlayerPointsNeededForBonusLife;
		mPlayerLives = Min(++mPlayerLives, mMaxPlayerLives);
	}

	if (mPlayerPoints > mMaxPlayerPoints)
	{
		mMaxPlayerPoints = mPlayerPoints;
		mHighScoreString = mPlayerPointsString;

		// Convert const char* -> const wchar_t*
		const char* highScore = mHighScoreString.c_str();
		const size_t len = std::strlen(highScore) + 1; // +1 for null terminator
		wchar_t* wstr = new wchar_t[len];
		std::mbstowcs(wstr, highScore, len);

		mHighScoreStringXOffset = 336.0f - (mFont->GetStringWidth(wstr, 25.0f) * 0.5f);

		// Free memory
		delete[] wstr;
	}
}

void GameManagerService::SetGhostChaseScatterMode(GhostMode mode)
{
	mGhostMode = mode;
	mBlinkyController->SetGhostMode(mGhostMode);
}

void GameManagerService::PlayAudioOneShot(const SAGE::Graphics::SoundId soundID)
{
	mSoundEffectManager->Play(soundID, false, mAudioVolume, Random::UniformFloat(-0.1f, 0.15f), Random::UniformFloat(-0.1f, 0.1f));
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
