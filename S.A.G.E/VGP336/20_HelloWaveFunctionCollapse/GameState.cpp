#include "GameState.h"

using namespace SAGE;
using namespace SAGE::Physics;
using namespace SAGE::Graphics;
using namespace SAGE::Math;
using namespace SAGE::Input;

namespace
{
	void DrawSprite(TextureId id, const Vector2& pos, float rotation = 0.0f, Pivot pivot = Pivot::Center, Flip flip = Flip::None)
	{
		SpriteRenderer::Get()->Draw(id, pos, rotation, pivot, flip);
	}
}

void GameState::Initialize()
{
	GraphicsSystem::Get()->SetClearColor(Colors::Gray);

	const auto& tm = TextureManager::Get();

	mMiniDesertID = tm->LoadTexture("../Sprites/WFCDemo/MiniDesert.png");
	mMiniForestID = tm->LoadTexture("../Sprites/WFCDemo/MiniForest.png");
	mMiniMountainsID = tm->LoadTexture("../Sprites/WFCDemo/MiniMountain.png");
	mMiniOceanID = tm->LoadTexture("../Sprites/WFCDemo/MiniOcean.png");
	mMiniPlainsID = tm->LoadTexture("../Sprites/WFCDemo/MiniPlains.png");
	mMiniSnowyTundraID = tm->LoadTexture("../Sprites/WFCDemo/MiniSnowyTundra.png");
	mMiniBlankID = tm->LoadTexture("../Sprites/WFCDemo/MiniBlank.png");

	mMaxiDesertID = tm->LoadTexture("../Sprites/WFCDemo/MaxiDesert.png");
	mMaxiForestID = tm->LoadTexture("../Sprites/WFCDemo/MaxiForest.png");
	mMaxiMountainsID = tm->LoadTexture("../Sprites/WFCDemo/MaxiMountain.png");
	mMaxiOceanID = tm->LoadTexture("../Sprites/WFCDemo/MaxiOcean.png");
	mMaxiPlainsID = tm->LoadTexture("../Sprites/WFCDemo/MaxiPlains.png");
	mMaxiSnowyTundraID = tm->LoadTexture("../Sprites/WFCDemo/MaxiSnowyTundra.png");
	mMaxiBlankID = tm->LoadTexture("../Sprites/WFCDemo/MaxiBlank.png");

	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			mSlots[x][y].position = Vector2(420.0f + 100.0f * x, 560.0f - 100.0f * y);
		}
	}

	mCamera.SetPosition({ 0.0f, 0.0f, -5.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
}

void GameState::Terminate()
{
	mMiniDesertID = 0;
	mMiniForestID = 0;
	mMiniMountainsID = 0;
	mMiniOceanID = 0;
	mMiniPlainsID = 0;
	mMiniSnowyTundraID = 0;
	mMiniBlankID = 0;

	mMaxiDesertID = 0;
	mMaxiForestID = 0;
	mMaxiMountainsID = 0;
	mMaxiOceanID = 0;
	mMaxiPlainsID = 0;
	mMaxiSnowyTundraID = 0;
	mMaxiBlankID = 0;
}

void GameState::Update(float deltaTime)
{
	// Step A
	// Go through each slot
	// Update it accordingly n, e, s, w
	// if any slot was updated repeat step A
	// else go to step B

	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			CheckNorth({ x, y }, mSlots[x][y]);
			CheckEast({ x, y }, mSlots[x][y]);
			CheckSouth({ x, y }, mSlots[x][y]);
			CheckWest({ x, y }, mSlots[x][y]);
		}
	}

	// Step B
	// Once there is no more slots to be updated choose the one with lowest entropy 
	// meaning least options remaining / randomly pick one if there are multiple slots with this value
	// Choose a biome for the slot
	// Repeat step A
	// If all slots are collapsed then be finished


}

void GameState::Render()
{
	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			if (mSlots[x][y].isCollapsed)
			{
				DrawSprite(mSlots[x][y].collapsedTexture, mSlots[x][y].position, 0.0f, Pivot::Center, Flip::None);
			}
			else
			{
				Vector2 pos = mSlots[x][y].position;
				DrawSprite(mSlots[x][y].isDesert ? mMiniDesertID : mMiniBlankID, pos + Vector2(-30.0f, -30.0f));
				DrawSprite(mSlots[x][y].isForest ? mMiniForestID : mMiniBlankID, pos + Vector2(0.0f, -30.0f));
				DrawSprite(mSlots[x][y].isMountains ? mMiniMountainsID : mMiniBlankID, pos + Vector2(30.0f, -30.0f));
				DrawSprite(mSlots[x][y].isOcean ? mMiniOceanID : mMiniBlankID, pos + Vector2(-30.0f, 0.0f));
				DrawSprite(mSlots[x][y].isPlains ? mMiniPlainsID : mMiniBlankID, pos);
				DrawSprite(mSlots[x][y].isSnowyTundra ? mMiniSnowyTundraID : mMiniBlankID, pos + Vector2(30.0f, 0.0f));
				DrawSprite(mMiniBlankID, pos + Vector2(-30.0f, 30.0f));
				DrawSprite(mMiniBlankID, pos + Vector2(0.0f, 30.0f));
				DrawSprite(mMiniBlankID, pos + Vector2(30.0f, 30.0f));
			}
		}
	}

	SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
}


void GameState::CheckNorth(Vector2Int position, Slot& slot)
{
	if (position.y + 1 >= 5) { return; }
	auto& northSlot = mSlots[position.x][position.y + 1];
	if (!northSlot.isCollapsed) { return; }

}

void GameState::CheckEast(Vector2Int position, Slot& slot)
{
	if (position.x + 1 >= 5) { return; }
	auto& eastSlot = mSlots[position.x + 1][position.y];
	if (!eastSlot.isCollapsed) { return; }

}

void GameState::CheckSouth(Vector2Int position, Slot& slot)
{
	if (position.y - 1 < 0) { return; }
	auto& southSlot = mSlots[position.x][position.y - 1];
	if (!southSlot.isCollapsed) { return; }

}

void GameState::CheckWest(Vector2Int position, Slot& slot)
{
	if (position.x - 1 < 0) { return; }
	auto& westSlot = mSlots[position.x - 1][position.y];
	if (!westSlot.isCollapsed) { return; }

}