#pragma once

#include "TypeIds.h"

class PlayerControllerComponent;

class PlayerAnimatorComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PlayerAnimator);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void Render();

private:
	float mTimer = 0.0f;
	float mTimePerFrame = 0.125f;
	int mSpriteIndex = 0;
	int mSpriteMaxIndex = 3;

	SAGE::Graphics::SpriteRenderer* spriteRenderer = nullptr;
	PlayerControllerComponent* mPlayerController = nullptr;
	std::vector<SAGE::Graphics::TextureId> mEatTextureIds;
	SAGE::Graphics::TextureId mDisplayTextureID = 0;
};