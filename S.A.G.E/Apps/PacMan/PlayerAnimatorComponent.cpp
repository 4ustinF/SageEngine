#include "PlayerAnimatorComponent.h"
#include "PlayerControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(PlayerAnimatorComponent, 1000);

void PlayerAnimatorComponent::Initialize()
{
	mPlayerController = GetOwner().GetComponent<PlayerControllerComponent>();

	auto tm = TextureManager::Get();
	mTextureID = tm->LoadTexture("../Sprites/PacMan/PacMan/Eat/sprite_eat_02.png");
}

void PlayerAnimatorComponent::Terminate()
{
	mPlayerController = nullptr;
	mTextureID = 0;
}

void PlayerAnimatorComponent::Update(float deltaTime)
{
}

void PlayerAnimatorComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Animator Component##PlayerAnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
	}
}

void PlayerAnimatorComponent::Render()
{
	const Vector2 pos = mPlayerController->GetPlayerPosition();
	SpriteRenderer::Get()->Draw(mTextureID, pos, 0.0, Pivot::Center, Flip::None);
}