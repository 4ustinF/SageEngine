#include "GhostControllerComponent.h"
#include "GameManagerService.h"
#include "TileMapService.h"

#include "PlayerControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Math::Random;
using namespace SAGE::Graphics;
using namespace SAGE::AI;

MEMORY_POOL_DEFINE(GhostControllerComponent, 500);

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

	// Teleport Tunnel
	if (mPosition.x <= -mHalfTileSize) {
		TeleportGhost({ mTunnelLimit, mPosition.y }, mDirection);
	}
	else if (mPosition.x >= mTunnelLimit) {
		TeleportGhost({ -mHalfTileSize, mPosition.y }, mDirection);
	}

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

	if (mGhostMode == GhostMode::Eaten) // Back at house
	{
		switch (mEatenState)
		{
		case GhostHomeState::GoingToHomeEntrance:
			if (IsRectOverlap(GetSmallColliderRect(), mHomeEntranceRect)) // Made it to home entrance
			{
				mPosition.x = 336.0f;
				mTargetPosition = { 336.0f, 420.f };
				mDirection = Direction::Down;
				mEatenState = GhostHomeState::GoingIntoHome;
			}
			break;
		case GhostHomeState::GoingIntoHome:
			if (IsRectOverlap(GetSmallColliderRect(), mHomeMidRect)) // Made it inside the home
			{
				mPosition.y = 420.0f;
				mTargetPosition = { 336.0f, 348.f };
				mDirection = Direction::Up;
				mEatenState = GhostHomeState::LeavingHome;

				mGhostMode = GhostMode::Scatter;
			}
			break;
		case GhostHomeState::LeavingHome:
			if (IsRectOverlap(GetSmallColliderRect(), mHomeEntranceRect)) // Made it out to the home entrance
			{
				mPosition.y = 348.0f;
				mEatenState = GhostHomeState::None;
				Respawn();
			}
			break;
		}

		//if (mTileCords == mHomeCords) // TODO: Find a better way to respawn at home
		//{
		//	Respawn();
		//}
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

		ImGui::DragInt2("Home Cords##GhostControllerComponent", &mCornerCords.x, 0.1f);
		//int temp = static_cast<int>(mGhostMode);
		//ImGui::DragInt("Ghost Mode", &temp, 0.5f);
	}
}


void GhostControllerComponent::Respawn()
{
	mGhostMode = GhostMode::Scatter;
	const Vector2 newPos = { 15.5f * mTileSize + mHalfTileSize + mWorldOffset.x, 11.0f * mTileSize + mHalfTileSize + mWorldOffset.y };
	TeleportGhost(newPos, Direction::Left);
}

void GhostControllerComponent::SetGhostMode(GhostMode mode)
{ 
	if (mGhostMode == mode) {
		return;
	}

	switch (mGhostMode)
	{
	case GhostMode::Chase:
	case GhostMode::Scatter:
		ReverseDirection();
		break;
	case GhostMode::Eaten:
		return; // Needs to go home first before changing ghost mode
	}

	mGhostMode = mode;
}

void GhostControllerComponent::SetGhostType(GhostType type)
{
	mGhostType = type;
	SetCornerCords();

	// TODO: set up ghost accordingly
	// Home cords
	// How to target? I don't think thats done here
}

void GhostControllerComponent::IsAten()
{
	mEatenState = GhostHomeState::GoingToHomeEntrance;
	SetGhostMode(GhostMode::Eaten);
}

void GhostControllerComponent::SetCornerCords()
{
	switch (mGhostType)
	{
	case GhostType::Blinky:
		mCornerCords = Vector2Int(28, 1);
		break;
	case GhostType::Pinky:
		mCornerCords = Vector2Int(3, 1);
		break;
	case GhostType::Inky:
		mCornerCords = Vector2Int(28, 29);
		break;
	case GhostType::Clyde:
		mCornerCords = Vector2Int(3, 29);
		break;
	}
}

void GhostControllerComponent::TeleportGhost(const Vector2 newPos, const Direction dir)
{
	mPosition = newPos;
	mDirection = dir;
	CalculateNewTargetPosition();
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

Vector2Int GhostControllerComponent::GetTargetCords() const
{
	switch (mGhostMode)
	{
	case GhostMode::Chase:
		return GetChaseTargetCords();
	case GhostMode::Scatter:
		return mCornerCords;
	case GhostMode::Eaten:
		return mHomeCords;
	}

	return mCornerCords;
}

Vector2Int GhostControllerComponent::GetChaseTargetCords() const
{
	switch (mGhostType)
	{
	case GhostType::Blinky: // pacmans position
		return mPlayerController->GetPlayerCords();
	case GhostType::Pinky: // 4 tiles in front of pacman. Or less if not valid. Can return pacmans position.
	{
		const Vector2Int dir = DirectionToVector2Int(mPlayerController->GetPlayerDirection());
		const Vector2Int playerCords = mPlayerController->GetPlayerCords();
		for (int i = 4; i >= 0; --i)
		{
			const Vector2Int targetCords = playerCords + (dir * i);
			if (!mTileMapService->IsBlocked(targetCords))
			{
				return targetCords;
			}
		}
		break;
	}
	case GhostType::Inky:
		// TODO:
		// Need pacman and Blinky to go off of.
		break;
	case GhostType::Clyde: // Pacmans position until too close. Then back to the corner. 
		const Vector2Int playerCords = mPlayerController->GetPlayerCords();
		const int dx = playerCords.x - mTileCords.x;
		const int dy = playerCords.y - mTileCords.y;
		if (std::abs(dx) + std::abs(dy) <= 8) {
			return mCornerCords;
		}
		return playerCords;
	}

	return mPlayerController->GetPlayerCords();
}


void GhostControllerComponent::CalculateNewTargetPosition()
{
	UpdateTileCords();

	// Check if its an intersection
	if (mGameManagerService->IsIntersectionPoint(mTileCords)) {

		if (mGhostMode == GhostMode::Frightened) {
			CalculateRandomDirection();
		}
		else {
			CalculateTargetPositionAtIntersection();
		}
	}
	else {
		CalculateTargetPositionContinuedDirection();
	}
}

void GhostControllerComponent::CalculateTargetPositionAtIntersection()
{
	const Vector2Int mEndPos = GetTargetCords();

	// Check if the target is within one tile away. 
	// Old ghost path finding method. Good for up close as it gives us the same result without more complexity.
	// We should use this if we don't find something below
	const int dx = mEndPos.x - mTileCords.x;
	const int dy = mEndPos.y - mTileCords.y;
	if (std::abs(dx) + std::abs(dy) <= 1) {
		if (std::abs(dx) > std::abs(dy)) {
			mDirection = dx > 0 ? Direction::Right : Direction::Left;
		}
		else {
			mDirection = dy > 0 ? Direction::Down : Direction::Up;
		}

		CalculateTargetPositionContinuedDirection();
		return;
	}

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

	// Get new target position
	if (!mTargetNodePositions.empty())
	{
		mPosition = mPosition;
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
}

void GhostControllerComponent::CalculateTargetPositionContinuedDirection()
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
			const Vector2 newPos{
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

void GhostControllerComponent::CalculateRandomDirection()
{
	const int dirSize = static_cast<int>(Direction::Size) - 1; // -1 because uniform int is inclusive
	while (true)
	{
		const Direction oppDir = GetOppositeDirection(mDirection);
		const Direction randNewDir = static_cast<Direction>(Random::UniformInt(0, dirSize));
		if (randNewDir != oppDir && !mTileMapService->IsBlocked(mTileCords + DirectionToVector2Int(randNewDir)))
		{
			mDirection = randNewDir;
			break;
		}
	}

	CalculateTargetPositionContinuedDirection();
}

void GhostControllerComponent::ReverseDirection()
{
	mDirection = GetOppositeDirection(mDirection);
	CalculateTargetPositionContinuedDirection();
}