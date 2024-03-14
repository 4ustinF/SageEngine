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
	BonusSymbol BonusSymbol = BonusSymbol::Cherries;
	float PacManSpeed = 0.0f;
	float GhostSpeed = 0.0f;
	float GhostTunnelSpeed = 0.0f;
	int Elroy1DotsLeft = 0;
	float Elroy1Speed = 0.0f;
	int Elroy2DotsLeft = 0;
	float Elroy2Speed = 0.0f;
	float FrightPacManSpeed = 0.0f;
	float FrightGhostSpeed = 0.0f;
	float FrightTime = 0.0f;
	int NumberOfFlashes = 0;
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

	void AteGhost();
	void CheckIfGhostAtePlayer();
	void CheckIfPlayerAteGhost();

	// References
	TileMapService* mTileMapService = nullptr;
	PlayerControllerComponent* mPlayerController = nullptr;
	PlayerAnimatorComponent* mPlayerAnimator = nullptr;
	SAGE::Coroutine::CoroutineSystem* mCoroutineSystem = nullptr;

	// Game
	int mLevel = 1;
	std::vector<Level> mLevels;
	Level mCurrentLevelData;
	const std::array<float, 7> mScatterChaseTimes = { 7.0f, 20.0f , 7.0f , 20.0f , 5.0f , 20.0f , 5.0f };
	int mScatterChaseIndex = 0;
	bool mTickScatterChaseTimer = true;
	float mScatterChaseTimer = 0.0f;
	float mFrightTimer = 0.0f;

	// Player
	const int mPlayerStartingLives = 3;
	int mPlayerLives = 3;
	int mPlayerPoints = 0;
	bool mIsInFrenzy = false;
	int mGhostAteInFrenzy = 0;
	const int mPointsPerGhostAte = 200;

	// Ghost
	void SetGhostChaseScatterMode(GhostMode mode);
	GhostControllerComponent* mBlinkyController = nullptr;
	GhostAnimatorComponent* mBlinkyAnimator = nullptr;
	GhostMode mGhostMode = GhostMode::Scatter;
	GhostMode mPrevGhostMode = GhostMode::Scatter;

	// Pellets
	const int mMaxPelletCount = 244;
	int mRemainingPelletCount = 244;
	std::vector<SAGE::Math::Vector2Int> mCachedSmallPelletCords;
	std::vector<SAGE::Math::Vector2Int> mCachedBigPelletCords;

	// Map
	std::vector<SAGE::Math::Vector2Int> mIntersections; // TODO: Don't use a vector. This uses linear look up times. Cord, pos
	std::vector<SAGE::Math::Vector2> mIntersectionsPositions;

	// Audio
	SAGE::Graphics::SoundEffectManager* mSoundEffectManager = nullptr;
	SAGE::Graphics::SoundId mMunchID;

	// Debug path finding
	SAGE::Graphics::TextureId mPathFindingTextureID = 0;

	// Coroutines
	SAGE::Coroutine::Enumerator GoToNextLevel();
};