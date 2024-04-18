#include "GameState.h"

#include "PipeComponent.h"

using namespace SAGE;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::ML;

namespace
{
	// Check for custom components
	bool OnMake(const char* componentName, const rapidjson::Value& value, GameObject& gameObject)
	{
		if (strcmp(componentName, "PipeComponent") == 0)
		{
			auto pipeComponent = gameObject.AddComponent<PipeComponent>();
			return true;
		}

		return false;
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/flappybird_level.json");

	mPipe1 = mGameWorld.FindGameObject("Pipe 1")->GetComponent<PipeComponent>();
	mPipe1->SetXPos(480.0f);

	mPipe2 = mGameWorld.FindGameObject("Pipe 2")->GetComponent<PipeComponent>();
	mPipe2->SetXPos(480.0f + 263.0f);

	mPipe3 = mGameWorld.FindGameObject("Pipe 3")->GetComponent<PipeComponent>();
	mPipe3->SetXPos(480.0f + 263.0f + 263.0f);

	mInputSystem = InputSystem::Get();
	mSpriteRenderer = SpriteRenderer::Get();

	auto tm = TextureManager::Get();
	mBackgroundTextureID = tm->LoadTexture("../Sprites/FlappyBird/background.png");
	mBaseTextureID = tm->LoadTexture("../Sprites/FlappyBird/base.png");
	mPipeTextureID = tm->LoadTexture("../Sprites/FlappyBird/pipe.png");
}

void GameState::Terminate()
{
	mPipeTextureID = 0;
	mBaseTextureID = 0;
	mBackgroundTextureID = 0;

	mPipe3 = nullptr;
	mPipe2 = nullptr;
	mPipe1 = nullptr;
	mSpriteRenderer = nullptr;
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	if (mInputSystem->IsKeyPressed(KeyCode::SPACE)) {
		mIsPaused = !mIsPaused;
	}

	if (mIsPaused) {
		return;
	}

	mGameWorld.Update(deltaTime);

	const float moveAmount = mMoveSpeed * deltaTime;
	mBase1XPos -= moveAmount;
	if (mBase1XPos <= -480.0f)
	{
		mBase1XPos += 960.0f;
	}

	mBase2XPos -= moveAmount;
	if (mBase2XPos <= -480.0f)
	{
		mBase2XPos += 960.0f;
	}
}

void GameState::Render()
{
	mGameWorld.Render();

	mSpriteRenderer->Draw(mBackgroundTextureID, Vector2::Zero, 0.0f, Pivot::TopLeft);

	mSpriteRenderer->Draw(mPipeTextureID, mPipe1->GetTopPipePos(), 0.0f, Pivot::BottomLeft, Flip::Vertical); // Top Pipe
	mSpriteRenderer->Draw(mPipeTextureID, mPipe1->GetBottomPipePos(), 0.0f, Pivot::TopLeft); // Bottom Pipe
	mSpriteRenderer->Draw(mPipeTextureID, mPipe2->GetTopPipePos(), 0.0f, Pivot::BottomLeft, Flip::Vertical); // Top Pipe
	mSpriteRenderer->Draw(mPipeTextureID, mPipe2->GetBottomPipePos(), 0.0f, Pivot::TopLeft); // Bottom Pipe
	mSpriteRenderer->Draw(mPipeTextureID, mPipe3->GetTopPipePos(), 0.0f, Pivot::BottomLeft, Flip::Vertical); // Top Pipe
	mSpriteRenderer->Draw(mPipeTextureID, mPipe3->GetBottomPipePos(), 0.0f, Pivot::TopLeft); // Bottom Pipe
	
	mSpriteRenderer->Draw(mBaseTextureID, Vector2(mBase1XPos, 853.0f), 0.0f, Pivot::BottomLeft);
	mSpriteRenderer->Draw(mBaseTextureID, Vector2(mBase2XPos, 853.0f), 0.0f, Pivot::BottomLeft);
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();

	ImGui::DragFloat("Base 1 Pos##Debug", &mBase1XPos, 1.0f, -500.0f, 500.0f);
	ImGui::DragFloat("Base 2 Pos##Debug", &mBase2XPos, 1.0f, -500.0f, 500.0f);

	if (ImGui::DragFloat("Move Speed##Debug", &mMoveSpeed, 1.0f, 0.0f, 500.0f))
	{
		mPipe1->SetPipeSpeed(mMoveSpeed);
		mPipe2->SetPipeSpeed(mMoveSpeed);
		mPipe3->SetPipeSpeed(mMoveSpeed);
	}
}
