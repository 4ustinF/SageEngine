#include "GhostControllerComponent.h"
#include "GameManagerService.h"
#include "TileMapService.h"

#include "PlayerControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::AI;

MEMORY_POOL_DEFINE(GhostControllerComponent, 1000);

void GhostControllerComponent::Initialize()
{
	auto& world = GetOwner().GetWorld();
	mTileMapService = world.GetService<TileMapService>();
	mGameManagerService = world.GetService<GameManagerService>();

	mTileSize = mTileMapService->GetTileSize();
	mHalfTileSize = mTileSize * 0.5f;
	mWorldOffset = mTileMapService->GetWorldOffset();

	// Player
	GameObject* playerObject = world.FindGameObject("PacMan");
	mPlayerController = playerObject->GetComponent<PlayerControllerComponent>();
}

void GhostControllerComponent::Terminate()
{
	mTargetNodePositions.clear();
	mPlayerController = nullptr;
	mTileMapService = nullptr;
	mGameManagerService = nullptr;
}

void GhostControllerComponent::Update(float deltaTime)
{
	UpdateTileCords();

	// Moves the ghost
	switch (mDirection)
	{
	case Direction::Up:
		if (mPosition.y > mTargetPosition.y) {
			mPosition.y -= mSpeed * deltaTime;
			if (mPosition.y <= mTargetPosition.y) {
				mPosition.y = mTargetPosition.y;
				CalculateNewTargetPosition();
			}
		}
		break;
	case Direction::Right:
		if (mPosition.x < mTargetPosition.x) {
			mPosition.x += mSpeed * deltaTime;
			if (mPosition.x >= mTargetPosition.x) {
				mPosition.x = mTargetPosition.x;
				CalculateNewTargetPosition();
			}
		}

		break;
	case Direction::Down:
		if (mPosition.y < mTargetPosition.y) {
			mPosition.y += mSpeed * deltaTime;
			if (mPosition.y >= mTargetPosition.y) {
				mPosition.y = mTargetPosition.y;
				CalculateNewTargetPosition();
			}
		}
		break;
	case Direction::Left:
		if (mPosition.x > mTargetPosition.x) {
			mPosition.x -= mSpeed * deltaTime;
			if (mPosition.x <= mTargetPosition.x) {
				mPosition.x = mTargetPosition.x;
				CalculateNewTargetPosition();
			}
		}
		break;
	}
}

void GhostControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Ghost Controller Component##GhostControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Speed##GhostControllerComponent", &mSpeed, 1.0f, 0.0f, 500.0f);
		ImGui::DragInt2("Tile Coordinate##GhostControllerComponent", &mTileCords.x, 0.1f);

		auto prevPos = mPosition;
		if (ImGui::DragFloat2("Position##GhostControllerComponent", &mPosition.x, 0.1f)) {
			if (mPosition.x != prevPos.x) {
				mDirection = mPosition.x > prevPos.x ? Direction::Right : Direction::Left;
			}
			else {
				mDirection = mPosition.y > prevPos.y ? Direction::Down : Direction::Up;
			}
		}
		ImGui::DragFloat2("Target Position##GhostControllerComponent", &mTargetPosition.x, 0.1f);
	}
}


void GhostControllerComponent::Respawn()
{
	const Vector2 newPos = { 3.0f * mTileSize + mHalfTileSize + mWorldOffset.x, 2.0f * mTileSize + mHalfTileSize + mWorldOffset.y };
	TeleportGhost(newPos, Direction::Right);
	CalculateNewTargetPosition();
}

void GhostControllerComponent::TeleportGhost(const Vector2 newPos, const Direction dir)
{
	mPosition = newPos;
	mDirection = dir;
	UpdateTileCords();
}

void GhostControllerComponent::UpdateTileCords()
{
	mTileCords.x = static_cast<int>((mPosition.x - mWorldOffset.x) / mTileSize);
	mTileCords.y = static_cast<int>((mPosition.y - mWorldOffset.y) / mTileSize);

	if (mOldTileCords != mTileCords)
	{
		mOldTileCords = mTileCords;
	}
}

void GhostControllerComponent::CalculateNewTargetPosition()
{
	// When you reach pass the target point
	UpdateTileCords();

	// Check if its an intersection
	if (mGameManagerService->IsIntersectionPoint(mTileCords)) // True: Path find - Set target position to first tile in path finding list
	{
		const Vector2Int mEndPos = mPlayerController->GetPlayerCords();

		// Get graph node we are on
		// Cache all neighbors
		// all neighbors null us
		// we null neighbors
		// restore
		// Get neighbor from the direction we are coming from
		
		// Block tile
		GridBasedGraph& graph = mTileMapService->mGridBasedGraph;
		Vector2Int blockTileCords = Vector2Int::Zero;
		switch (mDirection)
		{
		case Direction::Up:
			blockTileCords = { mTileCords.x, mTileCords.y + 1 };
			break;
		case Direction::Right:
			blockTileCords = { mTileCords.x - 1, mTileCords.y };
			break;
		case Direction::Down:
			blockTileCords = { mTileCords.x, mTileCords.y - 1 };
			break;
		case Direction::Left:
			blockTileCords = { mTileCords.x + 1, mTileCords.y };
			break;
		}
		graph.BlockTile(blockTileCords.x, blockTileCords.y);

		mTargetNodePositions = std::move(mTileMapService->FindPath(mTileCords.x, mTileCords.y, mEndPos.x, mEndPos.y));

		if (mTargetNodePositions.size() >= 2)
		{
			const Vector2& newPos = mTargetNodePositions[1] + mWorldOffset;

			if (newPos.x > mTargetPosition.x) {
				mDirection = Direction::Right;
			}
			else if (newPos.x < mTargetPosition.x) {
				mDirection = Direction::Left;
			}
			else if (newPos.y > mTargetPosition.y) {
				mDirection = Direction::Down;
			}
			else if (newPos.y < mTargetPosition.y) {
				mDirection = Direction::Up;
			}

			mTargetPosition = newPos;
		}

		// Unblock tile
		GridBasedGraph::Node* nodeToUnblock = graph.GetNode(blockTileCords.x, blockTileCords.y);
		
		if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y - 1)) { // North
			GridBasedGraph::Node* northNode = graph.GetNode(blockTileCords.x, blockTileCords.y - 1);
			northNode->neighbors[GridBasedGraph::South] = nodeToUnblock;
			nodeToUnblock->neighbors[GridBasedGraph::North] = northNode;
		}
		if (!mTileMapService->IsBlocked(mTileCords.x + 1, mTileCords.y)) { // Right
			GridBasedGraph::Node* eastNode = graph.GetNode(blockTileCords.x + 1, blockTileCords.y);
			eastNode->neighbors[GridBasedGraph::West] = nodeToUnblock;
			nodeToUnblock->neighbors[GridBasedGraph::East] = eastNode;
		}
		if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y + 1)) {
			GridBasedGraph::Node* southNode = graph.GetNode(blockTileCords.x, blockTileCords.y + 1);
			southNode->neighbors[GridBasedGraph::North] = nodeToUnblock;
			nodeToUnblock->neighbors[GridBasedGraph::South] = southNode;
		}
		if (!mTileMapService->IsBlocked(mTileCords.x - 1, mTileCords.y)) {
			GridBasedGraph::Node* westNode = graph.GetNode(blockTileCords.x - 1, blockTileCords.y);
			westNode->neighbors[GridBasedGraph::East] = nodeToUnblock;
			nodeToUnblock->neighbors[GridBasedGraph::West] = westNode;
		}
	}
	else // False: Try to keep going in the same direction - Can't go in the same direction. Check all other directions thats not behind you
	{
		switch (mDirection)
		{
		case Direction::Up:
			if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y - 1))
			{
				const Vector2 newPos{
					mTileCords.x * mTileSize + mWorldOffset.x + mHalfTileSize,
					(mTileCords.y - 1) * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x + 1, mTileCords.y))
			{
				const Vector2 newPos{
					(mTileCords.x + 1) * mTileSize + mWorldOffset.x + mHalfTileSize,
					mTileCords.y * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Right;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x - 1, mTileCords.y))
			{
				const Vector2 newPos{
					(mTileCords.x - 1) * mTileSize + mWorldOffset.x + mHalfTileSize,
					mTileCords.y * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Left;
			}
			break;
		case Direction::Right:
			if (!mTileMapService->IsBlocked(mTileCords.x + 1, mTileCords.y))
			{
				const Vector2 newPos {
					(mTileCords.x + 1) * mTileSize + mWorldOffset.x + mHalfTileSize,
					mTileCords.y * mTileSize + mWorldOffset.y + mHalfTileSize
				};
	
				mTargetPosition = newPos;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y + 1))
			{
				const Vector2 newPos{
					mTileCords.x * mTileSize + mWorldOffset.x + mHalfTileSize,
					(mTileCords.y + 1) * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Down;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y - 1))
			{
				const Vector2 newPos{
					mTileCords.x * mTileSize + mWorldOffset.x + mHalfTileSize,
					(mTileCords.y - 1) * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Up;
			}
			break;
		case Direction::Down:
			if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y + 1))
			{
				const Vector2 newPos{
					mTileCords.x * mTileSize + mWorldOffset.x + mHalfTileSize,
					(mTileCords.y + 1) * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x + 1, mTileCords.y))
			{
				const Vector2 newPos{
					(mTileCords.x + 1) * mTileSize + mWorldOffset.x + mHalfTileSize,
					mTileCords.y * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Right;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x - 1, mTileCords.y))
			{
				const Vector2 newPos{
					(mTileCords.x - 1) * mTileSize + mWorldOffset.x + mHalfTileSize,
					mTileCords.y * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Left;
			}
			break;
		case Direction::Left:
			if (!mTileMapService->IsBlocked(mTileCords.x - 1, mTileCords.y))
			{
				const Vector2 newPos{
					(mTileCords.x - 1) * mTileSize + mWorldOffset.x + mHalfTileSize,
					mTileCords.y * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y + 1))
			{
				const Vector2 newPos{
					mTileCords.x * mTileSize + mWorldOffset.x + mHalfTileSize,
					(mTileCords.y + 1) * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Down;
			}
			else if (!mTileMapService->IsBlocked(mTileCords.x, mTileCords.y - 1))
			{
				const Vector2 newPos{
					mTileCords.x * mTileSize + mWorldOffset.x + mHalfTileSize,
					(mTileCords.y - 1) * mTileSize + mWorldOffset.y + mHalfTileSize
				};

				mTargetPosition = newPos;
				mDirection = Direction::Up;
			}
			break;
		}
	}
}

void GhostControllerComponent::CalculateTargetNodePositions()
{
	// TODO: This is for debugging purposes
	const Vector2Int mEndPos = mPlayerController->GetPlayerCords();
	mTargetNodePositions = std::move(mTileMapService->FindPath(mTileCords.x, mTileCords.y, mEndPos.x, mEndPos.y));

	for (int i = 0; i < mTargetNodePositions.size(); ++i)
	{
		const Vector2& nodePos = mTargetNodePositions[i];
		const Vector2Int cords {
			static_cast<int>((nodePos.x - mWorldOffset.x) / mTileSize),
			static_cast<int>((nodePos.y - mWorldOffset.y) / mTileSize)
		};

		if (mGameManagerService->IsIntersectionPoint(cords))
		{
			mTargetNodePositions.erase(mTargetNodePositions.begin(), mTargetNodePositions.end());
		}
	}
	mTargetIndex = 0;
}

