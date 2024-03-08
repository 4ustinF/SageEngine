#include "GhostControllerComponent.h"
#include "TileMapService.h"
#include "GameManagerService.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(GhostControllerComponent, 1000);

void GhostControllerComponent::Initialize()
{
}

void GhostControllerComponent::Terminate()
{
}

void GhostControllerComponent::Update(float deltaTime)
{
}

void GhostControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Ghost Controller Component##GhostControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Speed", &mSpeed, 1.0f, 0.0f, 500.0f);
		auto prevPos = mPosition;
		if (ImGui::DragFloat2("Position##PlayerControllerComponent", &mPosition.x, 0.1f)) {
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
}
