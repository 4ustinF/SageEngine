#include "PlayerControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::Math;

MEMORY_POOL_DEFINE(PlayerControllerComponent, 1000);

void PlayerControllerComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	mAnimatorComponent = GetOwner().GetComponent<AnimatorComponent>();
}

void PlayerControllerComponent::Terminate()
{
}

void PlayerControllerComponent::Update(float deltaTime)
{
	auto inputSystem = InputSystem::Get();

	const float moveSpeed = inputSystem->IsKeyDown(KeyCode::LSHIFT) ? mRunSpeed : mWalkSpeed;

	Matrix4 transform = mTransformComponent->GetMatrix4();
	Vector3 velocity = Vector3::Zero;

	if (inputSystem->IsKeyDown(KeyCode::UP))
		velocity = GetLook(transform) * moveSpeed;
	if (inputSystem->IsKeyDown(KeyCode::DOWN))
		velocity = GetLook(transform) * -moveSpeed;
	if (inputSystem->IsKeyDown(KeyCode::RIGHT))
		velocity = GetRight(transform) * moveSpeed;
	if (inputSystem->IsKeyDown(KeyCode::LEFT))
		velocity = GetRight(transform) * -moveSpeed;

	Vector3 position = GetTranslation(transform);
	position += velocity * deltaTime;

	// Snap to terrain
	auto terrainService = GetOwner().GetWorld().GetService<TerrainService>();
	float height = terrainService->GetHeight(position);
	auto newPosition = Vector3{ position.x, height, position.z };
	mTransformComponent->position = newPosition;

	// Rotation
	if (inputSystem->IsMouseDown(MouseButton::RBUTTON))
	{
		auto mouseX = inputSystem->GetMouseMoveX() * deltaTime;
		auto rotation = Quaternion::RotationEuler({ 0.0f, mouseX, 0.0f });
		mTransformComponent->rotation = mTransformComponent->rotation * rotation;
	}

	auto& animator = mAnimatorComponent->GetAnimator();
	if (mAnimState != AnimState::Attack)
	{
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON)) {
			mAnimState = AnimState::Attack;
			animator.PlayAnimation(mAnimState, false);
		}
		else if (MagnitudeSqr(velocity) > 0.0f) {
			mAnimState = AnimState::Walk;
			animator.PlayAnimation(mAnimState);
		}
		else {
			mAnimState = AnimState::Idle;
			animator.PlayAnimation(mAnimState);
		}
	}
	else if(animator.IsFinished()) {
		mAnimState = AnimState::Idle;
		animator.PlayAnimation(mAnimState);
	}
}

void PlayerControllerComponent::DebugUI()
{
}