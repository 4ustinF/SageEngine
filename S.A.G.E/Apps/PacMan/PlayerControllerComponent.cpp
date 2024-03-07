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
	mWorldOffset = mTileMapService->GetWorldOffset();

	Respawn();
}

void PlayerControllerComponent::Terminate()
{
	mTileMapService = nullptr;
	mInputSystem = nullptr;
}

void PlayerControllerComponent::Update(float deltaTime)
{
	// Update tile cords
	mTileCords.x = static_cast<int>((mPosition.x - mWorldOffset.x) / mTileSize);
	mTileCords.y = static_cast<int>((mPosition.y - mWorldOffset.y) / mTileSize);

	// Eat pellet
	Tile& tile = mTileMapService->GetTile(mTileCords);
	if (tile.tileIndex == 1 || tile.tileIndex == 2)
	{
		tile.tileIndex = 0;
	}

	// Teleport Tunnel - TODO: Fix bug going through ghost box
	const float limit = 684.0f; // rows * TileSize
	if (mPosition.x <= -mHalfTileSize) {
		mPosition.x = limit;
	}
	else if (mPosition.x >= limit) {
		mPosition.x = -mHalfTileSize;
	}

	NewMovement(deltaTime);
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
	switch (mDirection)
	{
	case Direction::Up:
		for (int y = 1; y < mTileMapService->GetRows(); ++y)
		{
			int tileY = mTileCords.y - y;
			if (mTileMapService->IsBlocked(mTileCords.x, tileY++))
			{
				mTargetPosition.y = tileY * mTileSize + mHalfTileSize + mWorldOffset.y;
				mPosition.x = (mTileCords.x - 1) * mTileSize - mHalfTileSize; // Fix offset
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
				mTargetPosition.x = tileX * mTileSize - mHalfTileSize + mWorldOffset.x;
				mPosition.y = mTileCords.y * mTileSize - mHalfTileSize; // Fix offset
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
				mTargetPosition.y = tileY * mTileSize - mHalfTileSize + mWorldOffset.y;
				mPosition.x = (mTileCords.x - 1) * mTileSize - mHalfTileSize; // Fix offset
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
				mTargetPosition.x = tileX * mTileSize + mHalfTileSize + mWorldOffset.x;
				mPosition.y = mTileCords.y * mTileSize - mHalfTileSize; // Fix offset
				break;
			}
		}
		break;
	}
}

void PlayerControllerComponent::NewMovement(float deltaTime)
{
	// Player input
	if (mInputSystem->IsKeyPressed(KeyCode::UP) || mInputSystem->IsKeyPressed(KeyCode::W)) {
		mPreMoveDirection = Direction::Up;
	}
	else if (mInputSystem->IsKeyPressed(KeyCode::DOWN) || mInputSystem->IsKeyPressed(KeyCode::S)) {
		mPreMoveDirection = Direction::Down;
	}
	else if (mInputSystem->IsKeyPressed(KeyCode::RIGHT) || mInputSystem->IsKeyPressed(KeyCode::D)) {
		mPreMoveDirection = Direction::Right;
	}
	else if (mInputSystem->IsKeyPressed(KeyCode::LEFT) || mInputSystem->IsKeyPressed(KeyCode::A)) {
		mPreMoveDirection = Direction::Left;
	}

	// Turns the player
	if (mPreMoveDirection != Direction::None) // We have a set premove
	{
		if (IsOppositeDirection())
		{
			mDirection = mPreMoveDirection;
			mPreMoveDirection = Direction::None;
			SetDirection();
			return;
		}

		if (CanTurn())
		{
			mDirection = mPreMoveDirection;
			mPreMoveDirection = Direction::None;
			SetDirection();
		}
	}

	// Moves the player
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

bool PlayerControllerComponent::CanTurn()
{
	// Check if tile in that direction isBlocking or not
	switch (mPreMoveDirection)
	{
	case Direction::Up:
		if (mTileMapService->IsBlocked(mTileCords.x, mTileCords.y - 1)) {
			return false;
		}
		break;
	case Direction::Down:
		if (mTileMapService->IsBlocked(mTileCords.x, mTileCords.y + 1)) {
			return false;
		}
		break;
	case Direction::Right:
		if (mTileMapService->IsBlocked(mTileCords.x + 1, mTileCords.y)) {
			return false;
		}
		break;
	case Direction::Left:
		if (mTileMapService->IsBlocked(mTileCords.x - 1, mTileCords.y)) {
			return false;
		}
		break;
	}

	// If we line up with the tile
	switch (mPreMoveDirection)
	{
	case Direction::Up:
	case Direction::Down:
	{
		const float midX = mTileCords.x * mTileSize + mWorldOffset.x + mHalfTileSize;
		const float left = midX - mTurnThreshold;
		const float right = midX + mTurnThreshold;
		return left <= mPosition.x && mPosition.x <= right;
	}
	case Direction::Right:
	case Direction::Left:
	{
		const float midY = mTileCords.y * mTileSize + mWorldOffset.y + mHalfTileSize;
		const float up = midY - mTurnThreshold;
		const float down = midY + mTurnThreshold;
		return up <= mPosition.y && mPosition.y <= down;
	}
	}

	return false;
}

bool PlayerControllerComponent::IsOppositeDirection()
{
	switch (mDirection)
	{
	case Direction::Up:
		return mPreMoveDirection == Direction::Down;
	case Direction::Right:
		return mPreMoveDirection == Direction::Left;
	case Direction::Down:
		return mPreMoveDirection == Direction::Up;
	case Direction::Left:
		return mPreMoveDirection == Direction::Right;
	}
	return false;
}
