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

	spriteRenderer = SpriteRenderer::Get();

	auto tm = TextureManager::Get();
	mEatTextureIds.reserve(3);
	mEatTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/PacMan/Eat/sprite_eat_00.png"));
	mEatTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/PacMan/Eat/sprite_eat_01.png"));
	mEatTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/PacMan/Eat/sprite_eat_02.png"));

	mDisplayTextureID = mEatTextureIds[1];
	mTimer = mTimePerFrame;
}

void PlayerAnimatorComponent::Terminate()
{
	spriteRenderer = nullptr;

	mDisplayTextureID = 0;
	for (TextureId& id : mEatTextureIds) {
		id = 0;
	}
	mEatTextureIds.clear();

	mPlayerController = nullptr;
}

void PlayerAnimatorComponent::Update(float deltaTime)
{
	mTimer -= deltaTime;
	if (mTimer <= 0.0f)
	{
		if (mSpriteIndex == mSpriteMaxIndex)
		{
			mSpriteIndex = 0;
		}
		mDisplayTextureID = mEatTextureIds[mSpriteIndex++];
		mTimer += mTimePerFrame;
	}
}

void PlayerAnimatorComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Player Animator Component##PlayerAnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::InputFloat("Time Per Frame", &mTimePerFrame);
	}
}

void PlayerAnimatorComponent::Render()
{
	const Vector2 pos = mPlayerController->GetPlayerPosition();
	switch (mPlayerController->GetPlayerDirection())
	{
	case Direction::Up:
		spriteRenderer->Draw(mDisplayTextureID, pos, -Constants::HalfPi, Pivot::Center, Flip::None);
		break;
	case Direction::Right:
		spriteRenderer->Draw(mDisplayTextureID, pos, 0.0, Pivot::Center, Flip::None);
		break;
	case Direction::Down:
		spriteRenderer->Draw(mDisplayTextureID, pos, Constants::HalfPi, Pivot::Center, Flip::None);
		break;
	case Direction::Left:
		spriteRenderer->Draw(mDisplayTextureID, pos, 0.0, Pivot::Center, Flip::Horizontal);
		break;
	}
}
