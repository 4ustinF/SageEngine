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
	mFont = Font::Get();

	auto tm = TextureManager::Get();
	mTileTextureID = tm->LoadTexture("../Sprites/Nonogram/tile.png");
	mBackgroundTextureID = tm->LoadTexture("../Sprites/Nonogram/background.png");
	mBlackTileTextureID = tm->LoadTexture("../Sprites/Nonogram/blackTile.png");
	mBlackCrossTextureID = tm->LoadTexture("../Sprites/Nonogram/blackCross.png");
	mRedCrossTextureID = tm->LoadTexture("../Sprites/Nonogram/redCross.png");
	mMouseHoverTextureID = tm->LoadTexture("../Sprites/Nonogram/mouseHover.png");
}

void GameState::Terminate()
{
	mMouseHoverTextureID = 0;
	mRedCrossTextureID = 0;
	mBlackCrossTextureID = 0;
	mBlackTileTextureID = 0;
	mBackgroundTextureID = 0;
	mTileTextureID = 0;

	mFont = nullptr;
	mSpriteRenderer = nullptr;
	mInputSystem = nullptr;
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	if (mInputSystem->IsKeyPressed(KeyCode::P)) 
	{

	}

	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	//mGameWorld.Render();

	// Render background
	mSpriteRenderer->Draw(mBackgroundTextureID, Vector2(640.0f, 360.0f), 0.0f, Pivot::Center);

	// Render Play Space
	for (int x = 0; x < 3; ++x)
	{
		for (int y = 0; y < 3; ++y)
		{
			const float posX = x * 126.0f + 450.0f;
			const float posY = y * 126.0f + 170.0f;
			mSpriteRenderer->Draw(mTileTextureID, Vector2(posX, posY), 0.0f, Pivot::TopLeft); // 128 x 128
		}
	}

	// Render Text
	for (int i = 0; i < 15; ++i)
	{
		const float PosX = i * mFontExternalSpacing + mFontTopRowStartPos.x;
		const float PosY = i * mFontExternalSpacing + mFontLeftColumnStartPos.y;
		mFont->Draw("0", PosX, mFontTopRowStartPos.y, mFontSize, Colors::White);
		mFont->Draw("0", mFontLeftColumnStartPos.x, PosY, mFontSize, Colors::White);
	}

	for (int y = 0; y < 15; ++y)
	{
		for (int x = 0; x < 15; ++x)
		{
			switch (mCurrentBoardState[x][y])
			{
			case TileState::BlackCross:
					RenderTileState(x, y, mBlackCrossTextureID);
					break;
				case TileState::RedCross:
					RenderTileState(x, y, mRedCrossTextureID);
					break;
				case TileState::Filled:
					RenderTileState(x, y, mBlackTileTextureID);
					break;
			}
		}
	}
}

void GameState::DebugUI()
{
	mGameWorld.DebugUI();

	ImGui::DragFloat("mFontSize", &mFontSize, 1.0f, 0.0f, 50.0f);
	ImGui::DragFloat("mFontXSpacing", &mFontExternalSpacing, 1.0f, 0.0f, 50.0f);
	ImGui::DragFloat2("mFontTopRowStartPos", &mFontTopRowStartPos.x, 1.0f, 0.0f, 1290.0f);
	ImGui::DragFloat2("mFontLeftColumnStartPos", &mFontLeftColumnStartPos.x, 1.0f, 0.0f, 1290.0f);

	ImGui::DragFloat2("mBlackTileStartPos", &mBlackTileStartPos.x, 1.0f, 0.0f, 1290.0f);

	ImGui::DragInt("mTileTypeIndex", &mTileTypeIndex, 0.1f, 0, 3);
	ImGui::DragInt2("mSelectedTile", &mSelectedTile.x, 0.1f, 0, 14);

	if (ImGui::Button("Input Move"))
	{
		InputMove();
	}
}

void GameState::InputMove()
{
	// Don't input into empty space
	if (mCurrentBoardState[mSelectedTile.x][mSelectedTile.y] != TileState::Empty)
	{
		return;
	}

	// Check mSolvedBoardState if the move was correct
	// Set mCurrentBoardState accordingly
	// If the move was right and finished the quota fill the remaining spaces in that row or column accordingly with black Xs

	mCurrentBoardState[mSelectedTile.x][mSelectedTile.y] = static_cast<TileState>(mTileTypeIndex);
}

void GameState::RenderTileState(int x, int y, const std::size_t& textureID)
{
	const int posXAdjustment = x / 5;
	const int posYAdjustment = y / 5;
	const float blackPosX = x * mBlackTileSpriteSize + mBlackTileStartPos.x + posXAdjustment;
	const float blackPosY = y * mBlackTileSpriteSize + mBlackTileStartPos.y + posYAdjustment;
	mSpriteRenderer->Draw(textureID, Vector2(blackPosX, blackPosY), 0.0f, Pivot::TopLeft);
}