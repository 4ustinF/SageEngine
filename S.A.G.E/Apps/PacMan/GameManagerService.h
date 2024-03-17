#pragma once

#include "TypeIds.h"
#include "Enums.h"

class TileMapService;
class PlayerControllerComponent;
class PlayerAnimatorComponent;
class GhostControllerComponent;
class GhostAnimatorComponent;

struct Level
{
	BonusSymbol BonusSymbol;
	float PacManSpeed;
	float GhostSpeed;
	float GhostTunnelSpeed;		// (Ghosts slow down when traveling through the Warps on the sides)
	int Elroy1DotsLef;			// (When there are this many Dots left, Blinkies/Red Ghosts increase in speed)
	float Elroy1Speed;
	int Elroy2DotsLeft;			// (When there are this many Dots left, Blinkies/Red Ghosts increase in speed again)
	float Elroy2Speed;
	float FrightPacManSpeed;	// (after Pac-Man grabs a Power-Pellet)
	float FrightGhostSpeed;		// (the speed of blue vulnerable Ghosts)
	float FrightTime;			// (the time Ghosts spend in the blue vulnerable state)
	int NumberOfFlashes;		// (the visual indication that Fright Time for Ghosts is about to expire)
};

class GameManagerService final : public SAGE::Service
{
public:
	SET_TYPE_ID(ServiceId::GameManagerService);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

	void SetupGame();
	void StartGame();
	void AtePellet(PelletType pelletType);

	bool IsIntersectionPoint(SAGE::Math::Vector2Int pointToCheck) const;
	SAGE::Math::Vector2Int GetRandomPelletCord() const { return mCachedSmallPelletCords[SAGE::Math::Random::UniformInt(0, static_cast<int>(mCachedSmallPelletCords.size()) - 1)]; }

private:
	void CachePelletLocations();
	void RepopulatePellets();

	void RestartGame();
	void SetupLevel();
	void RestartLevel();

	void SetLevelData();
	void SetIntersectionPoints();

	void CheckIfGhostAtePlayer(const GhostControllerComponent* ghost);
	void CheckIfPlayerAteGhost(GhostControllerComponent* ghost);
	void CheckIfPlayerAteBonusSymbol();

	// References
	TileMapService* mTileMapService = nullptr;
	PlayerControllerComponent* mPlayerController = nullptr;
	PlayerAnimatorComponent* mPlayerAnimator = nullptr;
	SAGE::Coroutine::CoroutineSystem* mCoroutineSystem = nullptr;
	SAGE::Graphics::SpriteRenderer* mSpriteRenderer = nullptr;
	SAGE::Graphics::Font* mFont = nullptr;

	// Game
	int mLevel = 1;
	std::vector<Level> mLevels;
	Level mCurrentLevelData;
	const std::array<float, 7> mScatterChaseTimes = { 7.0f, 20.0f , 7.0f , 20.0f , 5.0f , 20.0f , 5.0f };
	int mScatterChaseIndex = 0;
	bool mTickScatterChaseTimer = true;
	float mScatterChaseTimer = 0.0f;
	float mFrightTimer = 0.0f;
	void AddIteration();
	int mNumOfIterations = 0;
	std::string mNumOfIterationsStr = "0";
	float mIterationXPos = 0.0f;

	// Player
	void AddPlayerPoints(int pointsToAdd);
	const int mPlayerStartingLives = 3;
	int mPlayerLives = 3;
	const int mMaxPlayerLives = 5;
	int mPlayerPoints = 0;
	int mMaxPlayerPoints = 0;
	bool mIsInFrenzy = false;
	int mGhostAteInFrenzy = 0;
	const int mPointsPerGhostAte = 200;
	int mPlayerPointsTillNextBonusLife = 0;
	const int mPlayerPointsNeededForBonusLife = 10000;

	// Ghost
	std::vector<GhostControllerComponent*> mGhostControllers; // TODO: Should be a map that contains both?
	std::vector<GhostAnimatorComponent*> mGhostAnimators;
	GhostMode mGhostMode = GhostMode::Scatter;
	GhostMode mPrevGhostMode = GhostMode::Scatter;

	// Ghost Eaten
	std::array<SAGE::Graphics::TextureId, 4> mEatenPointsTextureIDs;
	int mTextureIDIndex = 0;
	float mDisplayEatenPointsTimer = 0.0f;
	SAGE::Math::Vector2 mGhostEatenPosition = SAGE::Math::Vector2::Zero;

	// Pellets
	const int mMaxPelletCount = 244;
	int mRemainingPelletCount = 244;
	std::vector<SAGE::Math::Vector2Int> mCachedSmallPelletCords;
	std::vector<SAGE::Math::Vector2Int> mCachedBigPelletCords;

	// UI
	std::vector<SAGE::Graphics::TextureId> mBonusSymbolTextureIds;
	const float mFontSize = 25.0f;
	const float mBonusSymbolTextureIdsXSpacing = 46.0f;
	float mBonusSymbolTextureIdsXStartOffset = 0.0f;
	SAGE::Graphics::TextureId mPacmanLifeTextureID = 0;
	std::string mPlayerPointsString = "0";
	std::string mHighScoreString = "0";
	float mHighScoreStringXOffset = 330.0f; // mHalfScreenWidthX - half size("0")
	const float mHalfScreenWidthX = 336.0f;
	const float mScreenWidth = 672.0f; // We could set this with a get screen width call
	const float mPointsMaxTime = 1.0f;

	// Bonus Symbols - TODO: Maybe move to its own service
	bool mIsBonusSymbolActive = false;
	const SAGE::Math::Vector2 mBonusSymbolPosition = SAGE::Math::Vector2(338.0f, 492.0f); // TODO: Find bonus symbol position
	const SAGE::Math::Rect mBonusSymbolRect = { mBonusSymbolPosition.x - 2.0f, mBonusSymbolPosition.y - 2.0f, mBonusSymbolPosition.x + 2.0f, mBonusSymbolPosition.y + 2.0f };
	const int mBonusSymbol1RemainingPellets = 174; // 244 - 70
	const int mBonusSymbol2RemainingPellets = 74; // 244 -170
	const float mBonusSymbolMaxTime = 10.0f;
	float mBonusSymbolTimer = 0.0f;
	std::map<BonusSymbol, SAGE::Graphics::TextureId> mBonusSymbolTextureIDs;
	SAGE::Graphics::TextureId mBonusSymbolTextureID = 0;

	std::map<BonusSymbol, SAGE::Graphics::TextureId> mBonusSymbolsEatenPointsTextureIDsMap;
	float mDisplayBonusSymbolsEatenPointsTimer = 0.0f;

	// Map
	std::vector<SAGE::Math::Vector2Int> mIntersections; // TODO: Don't use a vector. This uses linear look up times. Cord, pos
	std::vector<SAGE::Math::Vector2> mIntersectionsPositions;

	// Audio
	void PlayAudioOneShot(const SAGE::Graphics::SoundId soundID);
	const float mAudioVolume = 0.5f;
	SAGE::Graphics::SoundEffectManager* mSoundEffectManager = nullptr;
	SAGE::Graphics::SoundId mMunchID = 0;
	SAGE::Graphics::SoundId mGhostEatenSoundID = 0;
	SAGE::Graphics::SoundId mBonusSymbolSoundID = 0;
	SAGE::Graphics::SoundId mExtraLifeSoundID = 0;

	// Debug path finding
	SAGE::Graphics::TextureId mPathFindingTextureID = 0;

	// Helper
	float StringToTextWidth(std::string text) const;

	// Coroutines
	SAGE::Coroutine::Enumerator GoToNextLevel();
};