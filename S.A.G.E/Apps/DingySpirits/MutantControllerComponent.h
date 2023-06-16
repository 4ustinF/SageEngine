#pragma once

#include "TypeIds.h"

class MutantControllerComponent final : public SAGE::Component
{
public:
	SET_TYPE_ID(ComponentId::MutantController);
	MEMORY_POOL_DECLARE;

	enum AnimState
	{
		Idle,
		Walk,
		Retreat,
		StrafeRight,
		StrafeLeft,
		Attack,
		Block
	};

	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void DebugUI() override;

	void SetWalkSpeed(float moveSpeed) { mWalkSpeed = moveSpeed; }
	void SetRunSpeed(float moveSpeed) { mRunSpeed = moveSpeed; }

private:
	SAGE::TransformComponent* mTransformComponent = nullptr;
	SAGE::AnimatorComponent* mAnimatorComponent = nullptr;
	AnimState mAnimState = AnimState::Idle;
	float mWalkSpeed = 1.0f;
	float mRunSpeed = 2.0f;
};