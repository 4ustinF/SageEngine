#pragma once

#include "TypeIds.h"

class PipeComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::PipeComponent);
	MEMORY_POOL_DECLARE;

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	// TODO: Rework
	SAGE::Math::Vector2 GetTopPipePos() { return { mPipeXPos, mPipeYPos.x }; }		// Bottom Left
	SAGE::Math::Vector2 GetBottomPipePos() { return { mPipeXPos, mPipeYPos.y }; }	// Top Left

	void SetXPos(float xPos) { mPipeXPos = xPos; }
	void SetPipeSpeed(float speed) { mPipeSpeed = speed; }
	bool CheckForBirdCollision(std::vector<SAGE::Math::Vector2> birdVerts);

private:
	void Reset();

	float mPipeSpeed = 100.0f;
	SAGE::Math::Vector2 mPipeYPos = SAGE::Math::Vector2::Zero;

	float mPipeXPos = 0.0f;

	const float mPipeWidth = 92.0f;
	const float mPipeHeight = 566.0f;
};