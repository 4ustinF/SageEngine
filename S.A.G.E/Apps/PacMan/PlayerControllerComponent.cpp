#include "PlayerControllerComponent.h"
#include "TileMapService.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::Math;

MEMORY_POOL_DEFINE(PlayerControllerComponent, 1000);

void PlayerControllerComponent::Initialize()
{
	mInputSystem = InputSystem::Get();

	mTileMapService = GetOwner().GetWorld().GetService<TileMapService>();
	mTileSize = mTileMapService->GetTileSize();
	mHalfTileSize = mTileSize * 0.5f;
	mStartingPosition = { 13.5f * mTileSize + mHalfTileSize, 23.0f * mTileSize + mHalfTileSize };

	Respawn();
}

void PlayerControllerComponent::Terminate()
{
	mTileMapService = nullptr;
	mInputSystem = nullptr;
}

void PlayerControllerComponent::Update(float deltaTime)
{
	const Vector2 tileOffset = mTileMapService->GetWorldOffset();
	mTileCords.x = static_cast<int>((mPosition.x - tileOffset.x) / mTileSize);
	mTileCords.y = static_cast<int>((mPosition.y - tileOffset.y) / mTileSize);

	// Eat pellet
	Tile& tile = mTileMapService->GetTile(mTileCords);
	if (tile.tileIndex == 1 || tile.tileIndex == 2)
	{
		tile.tileIndex = 0;
	}

	const float limit = 684.0f; // rows * TileSize
	if (mPosition.x < -mHalfTileSize) {
		mPosition.x = limit;
	}
	else if (mPosition.x >= limit) {
		mPosition.x = -mHalfTileSize;
	}

	if (mInputSystem->IsKeyDown(KeyCode::UP) || mInputSystem->IsKeyDown(KeyCode::W)) {
		mDirection = Direction::Up;
		SetDirection();
	}
	else if (mInputSystem->IsKeyDown(KeyCode::DOWN) || mInputSystem->IsKeyDown(KeyCode::S)) {
		mDirection = Direction::Down;
		SetDirection();
	}
	else if (mInputSystem->IsKeyDown(KeyCode::RIGHT) || mInputSystem->IsKeyDown(KeyCode::D)) {
		mDirection = Direction::Right;
		SetDirection();
	}
	else if (mInputSystem->IsKeyDown(KeyCode::LEFT) || mInputSystem->IsKeyDown(KeyCode::A)) {
		mDirection = Direction::Left;
		SetDirection();
	}

	switch (mDirection)
	{
	case Direction::Up:
		if (mPosition.y > mTargetPosition.y)
		{
			mPosition.y -= mSpeed * deltaTime;
			mPosition.y = Max(mPosition.y, mTargetPosition.y);
		}
		break;
	case Direction::Right:
		if (mPosition.x < mTargetPosition.x)
		{
			mPosition.x += mSpeed * deltaTime;
			mPosition.x = Min(mPosition.x, mTargetPosition.x);
		}
		break;
	case Direction::Down:
		if (mPosition.y < mTargetPosition.y)
		{
			mPosition.y += mSpeed * deltaTime;
			mPosition.y = Min(mPosition.y, mTargetPosition.y);
		}
		break;
	case Direction::Left:
		if (mPosition.x > mTargetPosition.x)
		{
			mPosition.x -= mSpeed * deltaTime;
			mPosition.x = Max(mPosition.x, mTargetPosition.x);
		}
		break;
	}
}

void PlayerControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Controller Component##PlayerControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Speed", &mSpeed, 1.0f, 0.0f, 500.0f);
		ImGui::DragFloat2("Position##PlayerControllerComponent", &mPosition.x, 0.1f);
		ImGui::DragFloat2("Target Position##PlayerControllerComponent", &mTargetPosition.x, 0.1f);
		ImGui::DragInt2("Tile Coordinate##PlayerControllerComponent", &mTileCords.x, 0.1f);
	}
}

void PlayerControllerComponent::Respawn()
{
	mPosition = mStartingPosition;
	mDirection = Direction::Right;
}

void PlayerControllerComponent::SetDirection()
{
	const Vector2 tileOffset = mTileMapService->GetWorldOffset();

	switch (mDirection)
	{
	case Direction::Up:
		for (int y = 1; y < mTileMapService->GetRows(); ++y)
		{
			int tileY = mTileCords.y - y;
			if (mTileMapService->IsBlocked(mTileCords.x, tileY++))
			{
				mTargetPosition.y = tileY * mTileSize + mHalfTileSize + tileOffset.y; // - -24
				//mPosition.x = mTileCords.x * mTileSize - mHalfTileSize; // Fix offset
				break;
			}
		}
		break;
	case Direction::Right:
		for (int x = 1; x < mTileMapService->GetColumns(); ++x)
		{
			const int tileX = mTileCords.x + x;
			if (mTileMapService->IsBlocked(tileX, mTileCords.y))
			{
				mTargetPosition.x = tileX * mTileSize - mHalfTileSize + tileOffset.x;
				//mPosition.y = mTileCords.y * mTileSize - mHalfTileSize; // Fix offset
				break;
			}
		}
		break;
	case Direction::Down:
		for (int y = 1; y < mTileMapService->GetRows(); ++y)
		{
			const int tileY = mTileCords.y + y;
			if (mTileMapService->IsBlocked(mTileCords.x, tileY))
			{
				mTargetPosition.y = tileY * mTileSize - mHalfTileSize + tileOffset.y;
				//mPosition.x = mTileCords.x * mTileSize - mHalfTileSize; // Fix offset
				break;
			}
		}
		break;
	case Direction::Left:
		for (int x = 1; x < mTileMapService->GetColumns(); ++x)
		{
			int tileX = mTileCords.x - x;
			if (mTileMapService->IsBlocked(tileX++, mTileCords.y))
			{
				mTargetPosition.x = tileX * mTileSize + mHalfTileSize + tileOffset.x;
				//mPosition.y = mTileCords.y * mTileSize - mHalfTileSize; // Fix offset
				break;
			}
		}
		break;
	}
}