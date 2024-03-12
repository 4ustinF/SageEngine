#pragma once

#include "TypeIds.h"
#include "Enums.h"

class GameManagerService;
class TileMapService;
class PlayerControllerComponent;

class GhostControllerComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::GhostController);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void Respawn();

	SAGE::Math::Vector2 GetPosition() const { return mPosition; }
	Direction GetDirection() const { return mDirection; }
	SAGE::Math::Vector2Int GetTileCords() const { return mTileCords; }
	std::vector<SAGE::Math::Vector2> mTargetNodePositions;

	void SetGhostMode(GhostMode mode) { mGhostMode = mode; }

	bool mIsChasing = false;

private:
	void SetHomeCords();

	void TeleportGhost(const SAGE::Math::Vector2 newPos, const Direction dir);
	void UpdateTileCords();

	SAGE::Math::Vector2Int GetTargetCords();
	void CalculateNewTargetPosition();
	void CalculateTargetPositionAtIntersection();
	void CalculateTargetPositionContinuedDirection();

	// References
	TileMapService* mTileMapService = nullptr;
	GameManagerService* mGameManagerService = nullptr;
	PlayerControllerComponent* mPlayerController = nullptr;

	SAGE::Math::Vector2 mPosition = SAGE::Math::Vector2::Zero;
	SAGE::Math::Vector2 mTargetPosition = SAGE::Math::Vector2::Zero;
	SAGE::Math::Vector2Int mTileCords = SAGE::Math::Vector2Int::Zero;
	SAGE::Math::Vector2Int mOldTileCords = SAGE::Math::Vector2Int::Zero;
	SAGE::Math::Vector2Int mHomeCords = SAGE::Math::Vector2Int::Zero;
	Direction mDirection = Direction::Right;
	float mSpeed = 140.0f;
	GhostType mGhostType = GhostType::Blinky;
	GhostMode mGhostMode = GhostMode::Scatter;

	// Target
	int mTargetIndex = 0;

	// Map Data
	float mTileSize = 0.0f;
	float mHalfTileSize = 0.0f;
	SAGE::Math::Vector2 mWorldOffset = SAGE::Math::Vector2::Zero;
	const float mTunnelLimit = 684.0f; // rows * TileSize

};