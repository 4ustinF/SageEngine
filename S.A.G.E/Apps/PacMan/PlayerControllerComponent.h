#pragma once

#include "TypeIds.h"
#include "Enums.h"

class TileMapService;
class GameManagerService;

class PlayerControllerComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PlayerController);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void Respawn();

	void SetPlayerSpeed(float multiplier) { mSpeed = mBaseSpeed * multiplier; }
	SAGE::Math::Vector2 GetPlayerPosition() const { return mPosition; }
	SAGE::Math::Vector2Int GetPlayerCords() const { return mTileCords; }
	Direction GetPlayerDirection() const { return mDirection; }
	SAGE::Math::Rect GetRect() const { return { mPosition.x - mHalfPacmanSpriteSize, mPosition.y - mHalfPacmanSpriteSize, mPosition.x + mHalfPacmanSpriteSize, mPosition.y + mHalfPacmanSpriteSize }; } // TODO: Set this value on move cause we might call this a lot. Especially for ghost collision.

	bool GetIsPlayerInvincible() const { return mIsInvincible; }

private:
	void UpdateTileCords();
	void Eating();
	void SetDirection();
	void TeleportPlayer(const SAGE::Math::Vector2 newPos, const Direction dir);

	void Movement(float deltaTime);
	bool CanTurn() const;
	bool IsOppositeDirection() const;
	float mTurnThreshold = 3.0f; // 4.0f

	// References
	TileMapService* mTileMapService = nullptr;
	GameManagerService* mGameManagerService = nullptr;
	SAGE::Input::InputSystem* mInputSystem = nullptr;

	SAGE::Math::Vector2 mStartingPosition = SAGE::Math::Vector2::Zero;
	SAGE::Math::Vector2 mPosition = SAGE::Math::Vector2::Zero;
	SAGE::Math::Vector2 mTargetPosition = SAGE::Math::Vector2::Zero;
	SAGE::Math::Vector2Int mTileCords = SAGE::Math::Vector2Int::Zero;
	Direction mDirection = Direction::Right;
	float mSpeed = 190.0f;
	const float mBaseSpeed = 190.0f;
	const float mTunnelLimit = 684.0f; // rows * TileSize

	Direction mPreMoveDirection = Direction::None;
	bool mIsInvincible = false;

	const float mPacmanSpriteSize = 39.0f;
	const float mHalfPacmanSpriteSize = 19.5f;

	// Map Data
	float mTileSize = 0.0f;
	float mHalfTileSize = 0.0f;
	SAGE::Math::Vector2 mWorldOffset = SAGE::Math::Vector2::Zero;
};