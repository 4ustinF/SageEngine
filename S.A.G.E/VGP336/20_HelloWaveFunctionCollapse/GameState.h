#include "Slot.h"

class GameState : public SAGE::AppState
{
public:
	void Initialize() override;
	void Terminate() override;

	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

	void CheckNorth(SAGE::Math::Vector2Int position, Slot& slot);
	void CheckEast(SAGE::Math::Vector2Int position, Slot& slot);
	void CheckSouth(SAGE::Math::Vector2Int position, Slot& slot);
	void CheckWest(SAGE::Math::Vector2Int position, Slot& slot);

private:
	SAGE::Graphics::Camera mCamera;

	Slot mSlots[5][5];

	SAGE::Graphics::TextureId mMiniDesertID = 0;
	SAGE::Graphics::TextureId mMiniForestID = 0;
	SAGE::Graphics::TextureId mMiniMountainsID = 0;
	SAGE::Graphics::TextureId mMiniOceanID = 0;
	SAGE::Graphics::TextureId mMiniPlainsID = 0;
	SAGE::Graphics::TextureId mMiniSnowyTundraID = 0;
	SAGE::Graphics::TextureId mMiniBlankID = 0;

	SAGE::Graphics::TextureId mMaxiDesertID = 0;
	SAGE::Graphics::TextureId mMaxiForestID = 0;
	SAGE::Graphics::TextureId mMaxiMountainsID = 0;
	SAGE::Graphics::TextureId mMaxiOceanID = 0;
	SAGE::Graphics::TextureId mMaxiPlainsID = 0;
	SAGE::Graphics::TextureId mMaxiSnowyTundraID = 0;
	SAGE::Graphics::TextureId mMaxiBlankID = 0;
};
