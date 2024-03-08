#include "GhostControllerComponent.h"
#include "TileMapService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(GhostControllerComponent, 1000);

void GhostControllerComponent::Initialize()
{
	mTileMapService = GetOwner().GetWorld().GetService<TileMapService>();

	mTileSize = mTileMapService->GetTileSize();
	mHalfTileSize = mTileSize * 0.5f;
	mWorldOffset = mTileMapService->GetWorldOffset();
}

void GhostControllerComponent::Terminate()
{
	mTileMapService = nullptr;
}

void GhostControllerComponent::Update(float deltaTime)
{
	UpdateTileCords();
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
	}
}


void GhostControllerComponent::Respawn()
{
	const Vector2 newPos = { 3.0f * mTileSize + mHalfTileSize + mWorldOffset.x, 2.0f * mTileSize + mHalfTileSize + mWorldOffset.y };
	TeleportGhost(newPos, Direction::Right);
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
}
