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
	mPosition = mStartPosition;
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
	mSpriteRenderer->Draw(mBirdTextureIDs[mAnimIndex], mPosition, mRotation, Pivot::Center, Flip::None);
}

void BirdControllerComponent::DebugUI()
{
	if (ImGui::CollapsingHeader("Bird Controller Component##BirdControllerComponent", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		ImGui::DragFloat2("Position##BirdControllerComponent", &mPosition.x, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Velocity##BirdControllerComponent", &mVelocity, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Min Velocity Cap##BirdControllerComponent", &mMinVelocityCap, 1.0f, -1000.0f, 0.0f);
		ImGui::DragFloat("Max Velocity Cap##BirdControllerComponent", &mMaxVelocityCap, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("Gravity##BirdControllerComponent", &mGravity, 1.0f, -1000.0f, 1000.0f);
		ImGui::DragFloat("Flap Amount##BirdControllerComponent", &mFlapAmount, 1.0f, 0.0f, 1000.0f);
		ImGui::DragFloat("Anim Time Per Frame##BirdControllerComponent", &mAnimTimePerFrame, 0.01f, 0.0f, 1.0f);
		if (ImGui::Checkbox("Is Dead##BirdControllerComponent", &mIsDead))
		{
			if (mIsDead) {
				KillBird();
			}
		}
	}
}

std::vector<Vector2> BirdControllerComponent::GetBirdVerts()
{
	std::vector<Vector2> birdVerts;
	birdVerts.reserve(4);

	birdVerts.push_back(Vector2(mPosition.x - mHalfWidth, mPosition.y - mHalfHeight)); // Top Left
	birdVerts.push_back(Vector2(mPosition.x + mHalfWidth, mPosition.y - mHalfHeight)); // Top Right
	birdVerts.push_back(Vector2(mPosition.x - mHalfWidth, mPosition.y + mHalfHeight)); // Bottom Left
	birdVerts.push_back(Vector2(mPosition.x + mHalfWidth, mPosition.y + mHalfHeight)); // Bottom Right

	return birdVerts;
}

void BirdControllerComponent::KillBird()
{
	mIsDead = true;
	mAnimIndex = 0;
}

void BirdControllerComponent::Flap()
{
	if (mIsDead) {
		return;
	}

	mVelocity -= mFlapAmount;
	mVelocity = Math::Clamp(mVelocity, mMinVelocityCap, mMaxVelocityCap);
}

void BirdControllerComponent::UpdateBirdAnimation(float deltaTime)
{
	if (mIsDead) {
		return;
	}

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
	if (mIsDead) { // Have the bird get cleared with the level
		mPosition.x -= 100.0f * deltaTime; // TODO: Map/Pipe speed
		mPosition.x = Math::Clamp(mPosition.x, -200.0f, 1000.0f); // TODO:: rework
	}

	// Position
	mPosition.y += mVelocity * deltaTime;
	mPosition.y = Math::Clamp(mPosition.y, 24.0f, 717.0f);

	// Rotation
	const float rotationAmount = Lerp(0.0f, 1.0f, (mVelocity - mMinVelocityCap) / (abs(mMinVelocityCap) + mMaxVelocityCap));
	const float targetRotation = Constants::DegToRad * Lerp(-60.0f, 90.0f, rotationAmount);
	mRotation = Lerp(mRotation, targetRotation, 0.1f);
}