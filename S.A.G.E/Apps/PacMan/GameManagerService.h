#pragma once

#include "TypeIds.h"
#include "Enums.h"

class TileMapService;
class PlayerControllerComponent;
class PlayerAnimatorComponent;
class GhostControllerComponent;
class GhostAnimatorComponent;

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

	void SetIntersectionPoints();

	// References
	TileMapService* mTileMapService = nullptr;
	PlayerControllerComponent* mPlayerController = nullptr;
	PlayerAnimatorComponent* mPlayerAnimator = nullptr;

	// Game
	int mLevel = 1;

	// Player
	const int mPlayerStartingLives = 3;
	int mPlayerLives = 3;
	int mPlayerPoints = 0;

	// Ghost
	GhostControllerComponent* mBlinkyController = nullptr;
	GhostAnimatorComponent* mBlinkyAnimator = nullptr;

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