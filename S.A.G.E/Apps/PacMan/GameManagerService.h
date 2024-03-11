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

private:
	void CachePelletLocations();
	void RepopulatePellets();

	void RestartGame();
	void SetupLevel();
	void RestartLevel();

	void SetLevelData();
	void SetIntersectionPoints();

	// References
	TileMapService* mTileMapService = nullptr;
	PlayerControllerComponent* mPlayerController = nullptr;
	PlayerAnimatorComponent* mPlayerAnimator = nullptr;
	SAGE::Coroutine::CoroutineSystem* mCoroutineSystem = nullptr;

	// Game
	int mLevel = 1;
	std::vector<Level> mLevels;

	// Player
	const int mPlayerStartingLives = 3;
	int mPlayerLives = 3;
	int mPlayerPoints = 0;
	bool mIsInFrenzy = false;

	// Ghost
	GhostControllerComponent* mBlinkyController = nullptr;
	GhostAnimatorComponent* mBlinkyAnimator = nullptr;
	bool mIsChasing = false;

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
	SAGE::Coroutine::Enumerator ScatterChaseWave();
};