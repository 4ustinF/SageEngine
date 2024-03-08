#pragma once

#include "TypeIds.h"
#include "Enums.h"

class TileMapService;
class GameManagerService;

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

private:

	SAGE::Math::Vector2 mPosition = SAGE::Math::Vector2::Zero;
	Direction mDirection = Direction::Right;
	float mSpeed = 150.0f;

};