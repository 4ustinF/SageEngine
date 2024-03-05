#pragma once

#include "TypeIds.h"
#include "Enums.h"

class TileMapService;

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

	SAGE::Math::Vector2 GetPlayerPosition() const { return mPosition; }
	Direction GetPlayerDirection() const { return mDirection; }

private:
	// References
	TileMapService* mTileMapService = nullptr;
	SAGE::Input::InputSystem* mInputSystem = nullptr;

	SAGE::Math::Vector2 mStartingPosition = SAGE::Math::Vector2::Zero;
	SAGE::Math::Vector2 mPosition = SAGE::Math::Vector2::Zero;
	Direction mDirection = Direction::Right;
	float mSpeed = 150.0f;

	float mTileSize = 0.0f;
	float mHalfTileSize = 0.0f;
};