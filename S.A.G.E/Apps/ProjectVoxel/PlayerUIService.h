#pragma once

#include "TypeIds.h"
#include "InventorySlot.h"
#include "RecipeOutcome.h"

class PlayerUIService final : public SAGE::Service
{
public:
	SET_TYPE_ID(ServiceId::PlayerUI);

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

	void DrawSprite(SAGE::Graphics::TextureId id, const SAGE::Math::Vector2& pos, float rotation = 0.0f, SAGE::Input::Pivot pivot = SAGE::Input::Pivot::Center, SAGE::Input::Flip flip = SAGE::Input::Flip::None);
	void DrawSprite(SAGE::Graphics::TextureId id, const SAGE::Math::Rect& sourceRect, const SAGE::Math::Vector2& pos, float rotation = 0.0f, SAGE::Input::Pivot pivot = SAGE::Input::Pivot::Center, SAGE::Input::Flip flip = SAGE::Input::Flip::None);

	void UpdateIconLocation(int index);

	void SetChunkCoord(SAGE::Math::Vector2Int chunkCoord) { mChunkCoord = chunkCoord; }
	void SetCurrentBiome(BiomeTypes currentBiome) { mCurrentBiome = currentBiome; }

	bool IsUIShowing() { return mUIShowing; }
	int GetIconSelected() { return mIconSelected; }

	void SetPlayerHandle(SAGE::GameObjectHandle playerHandle) { mPlayerGameObjectHandle = playerHandle; }

	void SwitchUIScreen(UIScreens newUIScreen);

	InventorySlot *playerInventory = nullptr;
	std::unordered_map<ItemTypes, SAGE::Graphics::TextureId> mItemTextureIds;

	void UpdatePlayerXP(int level, float xpPercentage);
	void UpdatePlayerHunger(float hungerPercentage);
	void UpdatePlayerHp(float hpPercentage);
	void UpdatePlayerArmor(int armorAmount);

private:
	const SAGE::CameraService* mCameraService = nullptr;
	SAGE::GameObjectHandle mPlayerGameObjectHandle;

	// TextureIds
	SAGE::Graphics::TextureId mCrossHairID = 0;
	SAGE::Graphics::TextureId mIconSelectID = 0;
	SAGE::Graphics::TextureId mXpBarEmptyID = 0;
	SAGE::Graphics::TextureId mXpBarFullID = 0;
	SAGE::Graphics::TextureId mItemBarIconID[9] = {0};
	SAGE::Graphics::TextureId mIconID = 0;
	SAGE::Graphics::TextureId mIconArrowID = 0;
	SAGE::Graphics::TextureId mIconFlameID = 0;
	SAGE::Graphics::TextureId mHealthBarEmptyID = 0;
	SAGE::Graphics::TextureId mHealthBarFullID = 0;	
	SAGE::Graphics::TextureId mArmorBarEmptyID = 0;
	SAGE::Graphics::TextureId mArmorBarFullID = 0;
	SAGE::Graphics::TextureId mHungerBarEmptyID = 0;
	SAGE::Graphics::TextureId mHungerBarFullID = 0;

	void IconHotBarUpdate(const SAGE::Input::InputSystem* inputSystem);
	int mIconSelected = 0;
	float mMouseZCache = 0.0f;
	SAGE::Math::Vector2 mIconSelectLocation;
	SAGE::Math::Vector2 mIconStartLocation;
	const float mIconOffSet = 80.0f;
	const float mItemBarY = 679.0f; //684.0f
	bool IsMouseHovering(const SAGE::Math::Vector2Int mousePosition, const SAGE::Math::Vector2 min, const SAGE::Math::Vector2 max);

	// Player HP
	void RenderHealth();
	float mHpBarCurrentSize = 329.0f;
	float mHpBarMaxSize = 329.0f;

	// Player Hunger
	void RenderHunger();
	float mHungerBarCurrentSize = 300.0f;
	float mHungerBarMaxSize = 300.0f;

	// Player XP
	void RenderXP();
	float mXPBarCurrentSize = 0.0f;
	float mXPBarMaxSize = 712.0f;
	int mPlayerLevel = 0;

	// Player Armor
	void RenderArmor();
	float mArmorBarCurrentSize = 0.0f;
	float mArmorBarMaxSize = 329.0f;

	// Inventory
	void ShowCreativeInventory();
	void ShowSurvialInventory();
	void ShowArmor(SAGE::Math::Vector2Int mousePosition);
	void InventoryOnSurvivalLeftClick(int index);
	void InventoryOnSurvivalRightClick(int index);
	void InventoryOnCreativeLeftClick(int index);
	void DisplayItemIcon(ItemTypes itemType, int amount, SAGE::Math::Vector2 position);
	bool mUIShowing = false;
	bool mItemInHand = false;
	SAGE::Math::Vector2 mInventoryPositions[27];
	SAGE::Math::Vector2 mItemBarPositions[9];
	const SAGE::Math::Rect mIconRect = SAGE::Math::Rect(0.0f, 0.0f, 72.0f, 72.0f);
	InventorySlot mInventorySlotInHand;
	int mSlotIndex = -1;
	int mCreativeIndex = 0;
	void InitializeItemTypeList();
	std::vector<ItemTypes> mItemTypesList;
	int mCreativeIndexCap = 1;
	void InitializeItemTextureIds();
	void ShowInventoryCrafting();
	void ShowCrafting();
	void UpdateCraftingProduceSlot();
	void UpdateFurnaceProduceSlot();

	void ReturnItemsInHand();
	void ReturnItemsInCraft();
	void ReturnItemsInSmelt();
	void ReturnItemsInFuel();

	void ShowFurnace();

	// Crafting recipies
	std::map<std::array<ItemTypes, 9>, RecipeOutcome> mCraftingRecipies;
	void InitializeCraftingRecipies();
	InventorySlot mCraftingSlots[9];
	InventorySlot mProduceSlot;
	InventorySlot mSmeltingSlot;
	InventorySlot mFuelSlot;
	InventorySlot mArmorSlots[4];

	// Debug
	void ShowDebug();
	bool mShowDebug = false;
	const float mFontSize = 25.0f;
	const SAGE::Graphics::Color mFontColor = SAGE::Graphics::Colors::Black;
	const char* mVersionText = "SageCraft - Version: 0.01";
	int mFPS = 0;
	const float mFpsTimer = 0.5f;
	float mFpsTime = 0.0f;
	SAGE::Math::Vector2Int mChunkCoord;
	BiomeTypes mCurrentBiome = BiomeTypes::Plains;

	// Temp
	UIScreens mUIScreens = UIScreens::None;
	void RenderUIScreen();
	void ExitUIScreen();

	void ShowGameplayHotBar();
	void HotBarLogic(SAGE::Math::Vector2Int mousePosition, SAGE::Math::Vector2 offSet, SAGE::Input::InputSystem* inputSystem);
};