#pragma once

#include "TypeIds.h"
#include "Enums.h"

class TileMapService;

class GameManagerService final : public SAGE::Service
{
public:
	SET_TYPE_ID(ServiceId::GameManagerService);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

	void AtePellet(PelletType pelletType);

private:
	void SetupLevel(int level);
	void RestartLevel();
	void RestartGame();

	// References
	TileMapService* mTileMapService = nullptr;

	// Game
	int mLevel = 1;

	// Player
	const int mPlayerStartingLives = 3;
	int mPlayerLives = 3;
	int mPlayerPoints = 0;

	// Pellets
	const int mMaxPelletCount = 244;
	int mRemainingPelletCount = 244;
};