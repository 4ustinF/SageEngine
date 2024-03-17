#include "GhostAnimatorComponent.h"
#include "GhostControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(GhostAnimatorComponent, 1000);

void GhostAnimatorComponent::Initialize()
{
	mGhostController = GetOwner().GetComponent<GhostControllerComponent>();

	mSpriteRenderer = SpriteRenderer::Get();

	mFrightenedTextureIds[0] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Frighten/sprite_00.png");
	mFrightenedTextureIds[1] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Frighten/sprite_01.png");
	mFrightenedTextureIds[2] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Frighten/sprite_02.png");
	mFrightenedTextureIds[3] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Frighten/sprite_03.png");

	mEatenTextureIds[0] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Eaten/sprite_up.png");
	mEatenTextureIds[1] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Eaten/sprite_right.png");
	mEatenTextureIds[2] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Eaten/sprite_down.png");
	mEatenTextureIds[3] = mTextureManager->LoadTexture("../Sprites/PacMan/Ghost/Eaten/sprite_left.png");

	mTimer = mTimePerFrame;
}

void GhostAnimatorComponent::Terminate()
{
	mTextureManager = nullptr;
	mSpriteRenderer = nullptr;

	mDisplayTextureID = 0;

	mGhostController = nullptr;
}

void GhostAnimatorComponent::Update(float deltaTime)
{
	if (mGhostController->GetGhostMode() == GhostMode::Eaten)
	{
		mDisplayTextureID = mEatenTextureIds[static_cast<int>(mGhostController->GetDirection())];
		return;
	}

	mTimer -= deltaTime;
	if (mTimer <= 0.0f)
	{
		if (mSpriteIndex == mSpriteMaxIndex) {
			mSpriteIndex = 0;
		}

		mDisplayTextureID = GetNewDisplayTexture();
		++mSpriteIndex;
		mTimer += mTimePerFrame;
	}
}

void GhostAnimatorComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Ghost Animator Component##GhostAnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::InputFloat("Time Per Frame", &mTimePerFrame);
		ImGui::DragInt("Sprite Index", &mSpriteIndex);
	}
}

void GhostAnimatorComponent::Render()
{
	mGhostController->GetGhostMode() == GhostMode::Eaten ? RenderEaten() : RenderMovement();
}

void GhostAnimatorComponent::InitGhostSprites(const std::array<std::filesystem::path, 6>& spriteFilePaths)
{
	mTextureManager = TextureManager::Get();

	// TODO: Don't separate these and we can for loop this.
	mMoveUpTextureIds[0] = mTextureManager->LoadTexture(spriteFilePaths[0]);
	mMoveUpTextureIds[1] = mTextureManager->LoadTexture(spriteFilePaths[1]);
	mMoveDownTextureIds[1] = mTextureManager->LoadTexture(spriteFilePaths[3]);
	mMoveDownTextureIds[0] = mTextureManager->LoadTexture(spriteFilePaths[2]);
	mMoveLeftTextureIds[0] = mTextureManager->LoadTexture(spriteFilePaths[4]);
	mMoveLeftTextureIds[1] = mTextureManager->LoadTexture(spriteFilePaths[5]);

	mDisplayTextureID = mMoveLeftTextureIds[0];
}

void GhostAnimatorComponent::RenderMovement()
{
	const Flip flip = mGhostController->GetDirection() == Direction::Right ? Flip::Horizontal : Flip::None;
	mSpriteRenderer->Draw(mDisplayTextureID, mGhostController->GetPosition(), 0.0, Pivot::Center, flip);
}

void GhostAnimatorComponent::RenderEaten()
{
	mSpriteRenderer->Draw(mDisplayTextureID, mGhostController->GetPosition(), 0.0, Pivot::Center, Flip::None);
}

TextureId GhostAnimatorComponent::GetNewDisplayTexture() const
{
	if (mGhostController->GetGhostMode() == GhostMode::Frightened)
	{
		return mFrightenedTextureIds[mSpriteIndex];
	}

	switch (mGhostController->GetDirection())
	{
	case Direction::Up:
		return mMoveUpTextureIds[mSpriteIndex];
	case Direction::Down:
		return mMoveDownTextureIds[mSpriteIndex];
	case Direction::Right:
	case Direction::Left:
		return mMoveLeftTextureIds[mSpriteIndex];
	}

	return mMoveUpTextureIds[mSpriteIndex];
}