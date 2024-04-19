#include "BirdControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Input;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

MEMORY_POOL_DEFINE(BirdControllerComponent, 300);

void BirdControllerComponent::Initialize()
{
	mInputSystem = InputSystem::Get();
	mSpriteRenderer = SpriteRenderer::Get();

	auto tm = TextureManager::Get();
	mBirdTextureIDs[0] = tm->LoadTexture("../Sprites/FlappyBird/bird1.png");
	mBirdTextureIDs[1] = tm->LoadTexture("../Sprites/FlappyBird/bird2.png");
	mBirdTextureIDs[2] = tm->LoadTexture("../Sprites/FlappyBird/bird3.png");

	mAnimTimer = mAnimTimePerFrame;
}

void BirdControllerComponent::Terminate()
{
	mBirdTextureIDs[0] = 0;
	mBirdTextureIDs[1] = 0;
	mBirdTextureIDs[2] = 0;

	mSpriteRenderer = nullptr;
	mInputSystem = nullptr;
}

void BirdControllerComponent::Update(float deltaTime)
{
	if (mInputSystem->IsKeyPressed(KeyCode::SPACE)) {
		Flap();
	}

	UpdateBirdAnimation(deltaTime);
	ApplyGravity(deltaTime);
	MoveBird(deltaTime);
}

void BirdControllerComponent::Render()
{
	mSpriteRenderer->Draw(mBirdTextureIDs[mAnimIndex], Vector2(240.0f, mPosition), mRotation, Pivot::Center, Flip::None);
}

void BirdControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Bird Controller Component##BirdControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat("Y Position##BirdControllerComponent", &mPosition, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Velocity##BirdControllerComponent", &mVelocity, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Min Velocity Cap##BirdControllerComponent", &mMinVelocityCap, 1.0f, -1000.0f, 0.0f);
		ImGui::DragFloat("Max Velocity Cap##BirdControllerComponent", &mMaxVelocityCap, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("Gravity##BirdControllerComponent", &mGravity, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Flap Amount##BirdControllerComponent", &mFlapAmount, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("mAnimTimePerFrame##BirdControllerComponent", &mAnimTimePerFrame, 0.01f, 0.0f, 1.0f);
	}
}

void BirdControllerComponent::Flap()
{
	mVelocity -= mFlapAmount;
	mVelocity = Math::Clamp(mVelocity, mMinVelocityCap, mMaxVelocityCap);
}

void BirdControllerComponent::UpdateBirdAnimation(float deltaTime)
{
	mAnimTimer -= deltaTime;
	if (mAnimTimer <= 0.0f)
	{
		if (++mAnimIndex == mMaxAnimIndex) {
			mAnimIndex = 0;
		}

		mAnimTimer += mAnimTimePerFrame;
	}
}

void BirdControllerComponent::ApplyGravity(float deltaTime)
{
	mVelocity += mGravity * deltaTime;
	mVelocity = Math::Clamp(mVelocity, mMinVelocityCap, mMaxVelocityCap);
}

void BirdControllerComponent::MoveBird(float deltaTime)
{
	// Position
	mPosition += mVelocity * deltaTime;
	mPosition = Math::Clamp(mPosition, 24.0f, 717.0f);

	// Rotation
	const float rotationAmount = Lerp(0.0f, 1.0f, (mVelocity - mMinVelocityCap) / (abs(mMinVelocityCap) + mMaxVelocityCap));
	const float targetRotation = Constants::DegToRad * Lerp(-60.0f, 90.0f, rotationAmount);
	mRotation = Lerp(mRotation, targetRotation, 0.1f);
}