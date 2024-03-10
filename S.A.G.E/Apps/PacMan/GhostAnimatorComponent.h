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

private:
	float mTimer = 0.0f;
	float mTimePerFrame = 0.15f;
	int mSpriteIndex = 0;
	int mSpriteMaxIndex = 2;

	std::vector<SAGE::Graphics::TextureId>& GetTextureIDLookup();

	GhostControllerComponent* mGhostController = nullptr;
	std::vector<SAGE::Graphics::TextureId> mMoveUpTextureIds;
	std::vector<SAGE::Graphics::TextureId> mMoveDownTextureIds;
	std::vector<SAGE::Graphics::TextureId> mMoveLeftTextureIds;
	SAGE::Graphics::TextureId mDisplayTextureID = 0;
};