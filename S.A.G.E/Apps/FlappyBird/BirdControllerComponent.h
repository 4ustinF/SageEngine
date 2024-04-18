#pragma once

#include "TypeIds.h"

class BirdControllerComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::BirdControllerComponent);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render();
	void DebugUI() override;


private:

	void Flap();
	void UpdateBirdAnimation(float deltaTime);
	void ApplyGravity(float deltaTime);
	void MoveBird(float deltaTime);

	// References
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::Graphics::SpriteRenderer* mSpriteRenderer = nullptr;

	// Animation
	SAGE::Graphics::TextureId mBirdTextureIDs[3];
	const int mMaxAnimIndex = 3;
	int mAnimIndex = 0;
	float mAnimTimer = 0.0f;
	float mAnimTimePerFrame = 0.15f;

	float mPosition = 426.5f;
	float mVelocity = 0.0f;
	float mMinVelocityCap = -375.0f; // Going Up
	float mMaxVelocityCap = 475.0f; // Going Down
	float mGravity = 1250.0f;
	float mFlapAmount = 500.0f;
};