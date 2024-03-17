#pragma once

#include "TypeIds.h"

class GhostControllerComponent;

class GhostAnimatorComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::GhostAnimator);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void Render();

	void InitGhostSprites(const std::array<std::filesystem::path, 6>& spriteFilePaths);

private:
	void RenderMovement();
	void RenderEaten();

	// References
	SAGE::Graphics::SpriteRenderer* mSpriteRenderer = nullptr;
	SAGE::Graphics::TextureManager* mTextureManager = nullptr;
	GhostControllerComponent* mGhostController = nullptr;

	float mTimer = 0.0f;
	float mTimePerFrame = 0.15f;
	int mSpriteIndex = 0;
	int mSpriteMaxIndex = 2;


	SAGE::Graphics::TextureId GetNewDisplayTexture() const;
	std::array<SAGE::Graphics::TextureId, 2> mMoveUpTextureIds;
	std::array<SAGE::Graphics::TextureId, 2> mMoveDownTextureIds;
	std::array<SAGE::Graphics::TextureId, 2> mMoveLeftTextureIds;
	std::array<SAGE::Graphics::TextureId, 4> mFrightenedTextureIds;
	std::array<SAGE::Graphics::TextureId, 4> mEatenTextureIds;
	SAGE::Graphics::TextureId mDisplayTextureID = 0;
};