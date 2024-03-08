#pragma once

#include "TypeIds.h"
#include "Enums.h"

class TileMapService;

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

	SAGE::Math::Vector2 GetGhostPosition() const { return mPosition; }
	Direction GetGhostDirection() const { return mDirection; }
	SAGE::Math::Vector2Int GetGhostTileCords() const { return mTileCords; }

	SAGE::Math::Vector2 mPosition = SAGE::Math::Vector2::Zero;

private:
	void TeleportGhost(const SAGE::Math::Vector2 newPos, const Direction dir);
	void UpdateTileCords();

	// References
	TileMapService* mTileMapService = nullptr;

	SAGE::Math::Vector2Int mTileCords = SAGE::Math::Vector2Int::Zero;
	Direction mDirection = Direction::Right;
	float mSpeed = 150.0f;

	// Map Data
	float mTileSize = 0.0f;
	float mHalfTileSize = 0.0f;
	SAGE::Math::Vector2 mWorldOffset = SAGE::Math::Vector2::Zero;

};