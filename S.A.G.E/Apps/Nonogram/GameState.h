#include <SAGE/Inc/SAGE.h>
#include "Enums.h"

class BirdControllerComponent;
class PipeComponent;

class GameState : public SAGE::AppState
{
public:
	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	// References
	SAGE::GameWorld mGameWorld;
	SAGE::Input::InputSystem* mInputSystem = nullptr;
	SAGE::Graphics::SpriteRenderer* mSpriteRenderer = nullptr;
	SAGE::Graphics::Font* mFont = nullptr;

	SAGE::Graphics::TextureId mTileTextureID = 0;
	SAGE::Graphics::TextureId mBackgroundTextureID = 0;
	SAGE::Graphics::TextureId mBlackTileTextureID = 0;
	SAGE::Graphics::TextureId mBlackCrossTextureID = 0;
	SAGE::Graphics::TextureId mRedCrossTextureID = 0;
	SAGE::Graphics::TextureId mMouseHoverTextureID = 0;

	float mFontSize = 18.0f;
	float mFontInternalSpacing = 25.2f;
	float mFontExternalSpacing = 25.2f;
	SAGE::Math::Vector2 mFontTopRowStartPos = SAGE::Math::Vector2(458.0f, 144.0f);
	SAGE::Math::Vector2 mFontLeftColumnStartPos = SAGE::Math::Vector2(430.0f, 172.0f);

	// Game information
	TileState mSolvedBoardState[15][15] = {};
	TileState mCurrentBoardState[15][15] = {};

	// Black Tiles
	SAGE::Math::Vector2 mBlackTileStartPos = SAGE::Math::Vector2(452.0f, 172.0f);
	const float mBlackTileSpriteSize = 25.0f;

	// Helper Functions
	void RenderTileState(int x, int y, const std::size_t& textureID);
	void InputMove();

	int mTileTypeIndex = 0;
	SAGE::Math::Vector2Int mSelectedTile = SAGE::Math::Vector2Int::Zero;
};