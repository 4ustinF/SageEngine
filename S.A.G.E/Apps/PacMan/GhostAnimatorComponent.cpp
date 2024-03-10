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

	// TODO: Set textures through json
	auto tm = TextureManager::Get();
	mMoveUpTextureIds.reserve(2);
	mMoveUpTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/Ghost/Blinky/sprite_up_00.png"));
	mMoveUpTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/Ghost/Blinky/sprite_up_01.png"));
	mMoveDownTextureIds.reserve(2);
	mMoveDownTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/Ghost/Blinky/sprite_down_00.png"));
	mMoveDownTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/Ghost/Blinky/sprite_down_01.png"));
	mMoveLeftTextureIds.reserve(2);
	mMoveLeftTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/Ghost/Blinky/sprite_left_00.png"));
	mMoveLeftTextureIds.push_back(tm->LoadTexture("../Sprites/PacMan/Ghost/Blinky/sprite_left_01.png"));

	mDisplayTextureID = mMoveLeftTextureIds[0];
	mTimer = mTimePerFrame;
}

void GhostAnimatorComponent::Terminate()
{
	mDisplayTextureID = 0;
	for (TextureId& id : mMoveUpTextureIds) { id = 0; }
	mMoveUpTextureIds.clear();
	for (TextureId& id : mMoveDownTextureIds) { id = 0; }
	mMoveDownTextureIds.clear();
	for (TextureId& id : mMoveLeftTextureIds) { id = 0; }
	mMoveLeftTextureIds.clear();

	mGhostController = nullptr;
}

void GhostAnimatorComponent::Update(float deltaTime)
{
	mTimer -= deltaTime;
	if (mTimer <= 0.0f)
	{
		if (mSpriteIndex == mSpriteMaxIndex)
		{
			mSpriteIndex = 0;
		}
		mDisplayTextureID = GetTextureIDLookup()[mSpriteIndex++];
		mTimer += mTimePerFrame;
	}
}

void GhostAnimatorComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Ghost Animator Component##GhostAnimatorComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::InputFloat("Time Per Frame", &mTimePerFrame);
	}
}

void GhostAnimatorComponent::Render()
{
	const Flip flip = mGhostController->GetDirection() == Direction::Right ? Flip::Horizontal : Flip::None;
	SpriteRenderer::Get()->Draw(mDisplayTextureID, mGhostController->GetPosition(), 0.0, Pivot::Center, flip);
}

std::vector<TextureId>& GhostAnimatorComponent::GetTextureIDLookup()
{
	switch (mGhostController->GetDirection())
	{
	case Direction::Up:
		return mMoveUpTextureIds;
	case Direction::Down:
		return mMoveDownTextureIds;
	case Direction::Right:
	case Direction::Left:
		return mMoveLeftTextureIds;
	}

	return mMoveUpTextureIds;
}
