#include "MutantControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::Math;

MEMORY_POOL_DEFINE(MutantControllerComponent, 1000);

void MutantControllerComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	mAnimatorComponent = GetOwner().GetComponent<AnimatorComponent>();
}

void MutantControllerComponent::Terminate()
{

}

void MutantControllerComponent::Update(float deltaTime)
{
	auto player = GetOwner().GetWorld().FindGameObject("Player");
	if (player == nullptr) { return; }

	const float moveSpeed = mWalkSpeed;
	const float turnSpeed = 0.1f;

	auto playerTransformComponent = player->GetComponent<TransformComponent>();

	Vector3 mutantToPlayer = playerTransformComponent->position - mTransformComponent->position;
	Vector3 direction = Normalize(mutantToPlayer);
	Vector3 velocity = direction * moveSpeed;

	Matrix4 transform = mTransformComponent->GetMatrix4();
	Vector3 position = GetTranslation(transform);
	position += velocity * deltaTime;

	// Snap to terrain
	auto terrainService = GetOwner().GetWorld().GetService<TerrainService>();
	float height = terrainService->GetHeight(position);
	auto newPosition = Vector3{ position.x, height, position.z };
	mTransformComponent->position = newPosition;

	// Rotation
	mTransformComponent->rotation = Quaternion::RotationLook(direction);

	auto& animator = mAnimatorComponent->GetAnimator();
	if (MagnitudeSqr(velocity) > 0.0f) {
		mAnimState = AnimState::Walk;
		animator.PlayAnimation(mAnimState);
	}
	else {
		mAnimState = AnimState::Idle;
		animator.PlayAnimation(mAnimState);
	}
}

void MutantControllerComponent::DebugUI()
{

}