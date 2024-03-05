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
	mStartingPosition = {13.5f * mTileSize + mHalfTileSize, 23.0f * mTileSize + mHalfTileSize };

	Respawn();
}

void PlayerControllerComponent::Terminate()
{
	mTileMapService = nullptr;
	mInputSystem = nullptr;
}

void PlayerControllerComponent::Update(float deltaTime)
{
	if (mInputSystem->IsKeyDown(KeyCode::UP)) 
	{
		//const int tilePosX = static_cast<int>(mPosition.x / mTileSize);
		//const int tilePosY = static_cast<int>((mPosition.y / mTileSize) + mHalfTileSize);

		//if (mTileMapService->IsBlocked(tilePosX, tilePosY))
		//{
		//	mPosition.y -= mSpeed * deltaTime;
		//}
		mPosition.y -= mSpeed * deltaTime;
	}
	else if (mInputSystem->IsKeyDown(KeyCode::DOWN)) {
		mPosition.y += mSpeed * deltaTime;
	}
	else if (mInputSystem->IsKeyDown(KeyCode::RIGHT)) {
		mPosition.x += mSpeed * deltaTime;
	}
	else if (mInputSystem->IsKeyDown(KeyCode::LEFT)) {
		mPosition.x -= mSpeed * deltaTime;
	}

}

void PlayerControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Controller Component##PlayerControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Speed", &mSpeed, 1.0f, 0.0f, 500.0f);
	}
}

void PlayerControllerComponent::Respawn()
{
	mPosition = mStartingPosition;
	mDirection = Direction::Right;
}