#include "GameState.h"

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
		return false;
	}
}

void GameState::Initialize()
{
	mGameWorld.AddService<CameraService>();
	mGameWorld.AddService<RenderService>();
	mGameWorld.Initialize(1000);

	GameObjectFactory::SetMakeOverride(OnMake);
	mGameWorld.LoadLevel("../../Assets/Level/bare.json");

	mInputSystem = InputSystem::Get();
	mSpriteRenderer = SpriteRenderer::Get();


	auto tm = TextureManager::Get();
	mBackgroundTextureID = tm->LoadTexture("../Sprites/FlappyBird/background.png");
	mBaseTextureID = tm->LoadTexture("../Sprites/FlappyBird/base.png");
}

void GameState::Terminate()
{
	mBackgroundTextureID = 0;
	mBaseTextureID = 0;

	mSpriteRenderer = nullptr;
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	if (mInputSystem->IsKeyPressed(KeyCode::SPACE)) {
		mIsPaused = !mIsPaused;
	}

	if (!mIsPaused) {
		mGameWorld.Update(deltaTime);
	}
}

void GameState::Render()
{
	mGameWorld.Render();

	mSpriteRenderer->Draw(mBackgroundTextureID, Vector2::Zero, 0.0f, Pivot::TopLeft);
	mSpriteRenderer->Draw(mBaseTextureID, Vector2(0.0f, 853.0f), 0.0f, Pivot::BottomLeft);
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();
}
