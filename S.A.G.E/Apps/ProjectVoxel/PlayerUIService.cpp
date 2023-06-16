#include "PlayerUIService.h"
#include "WorldService.h"

#include "PlayerControllerComponent.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;
using namespace SAGE::Input;
namespace rj = rapidjson;

void PlayerUIService::DrawSprite(TextureId id, const Vector2& pos, float rotation, Pivot pivot, Flip flip)
{
	SpriteRenderer::Get()->Draw(id, pos, rotation, pivot, flip);
}

void PlayerUIService::DrawSprite(TextureId id, const Rect& sourceRect, const Vector2& pos, float rotation, Pivot pivot, Flip flip)
{
	SpriteRenderer::Get()->Draw(id, sourceRect, pos, rotation, pivot, flip);
}

void PlayerUIService::Initialize()
{
	SetServiceName("PlayerUI Service");
	mCameraService = GetWorld().GetService<CameraService>();

	auto tm = TextureManager::Get();
	mCrossHairID = tm->LoadTexture("../Sprites/SageCraft/GUI/CrossHair.png");
	mIconSelectID = tm->LoadTexture("../Sprites/SageCraft/GUI/IconSelect.png");
	mXpBarEmptyID = tm->LoadTexture("../Sprites/SageCraft/GUI/XpBarEmpty.png");
	mXpBarFullID = tm->LoadTexture("../Sprites/SageCraft/GUI/XpBarFull.png");
	mIconID = tm->LoadTexture("../Sprites/SageCraft/GUI/Icon.png");
	mIconArrowID = tm->LoadTexture("../Sprites/SageCraft/GUI/Icon_Arrow.png");
	mIconFlameID = tm->LoadTexture("../Sprites/SageCraft/GUI/Icon_Flame.png");
	mHealthBarEmptyID = tm->LoadTexture("../Sprites/SageCraft/GUI/HealthBarEmpty.png");
	mHealthBarFullID = tm->LoadTexture("../Sprites/SageCraft/GUI/HealthBarFull.png");
	mArmorBarEmptyID = tm->LoadTexture("../Sprites/SageCraft/GUI/ArmorBarEmpty.png");
	mArmorBarFullID = tm->LoadTexture("../Sprites/SageCraft/GUI/ArmorBarFull.png");
	mHungerBarEmptyID = tm->LoadTexture("../Sprites/SageCraft/GUI/HungerBarEmpty.png");
	mHungerBarFullID = tm->LoadTexture("../Sprites/SageCraft/GUI/HungerBarFull.png");

	InitializeItemTextureIds();

	mIconSelectLocation = { 320.0f, mItemBarY };
	mIconStartLocation = mIconSelectLocation;

	int count = 0;
	for (int rows = 0; rows < 3; ++rows)
	{
		for (int columns = 0; columns < 9; ++columns)
		{
			//mInventoryPositions[count++] = Vector2(320.0f + 80.0f * columns, 560.0f - 80.0f * rows);  // bot left Right and up
			mInventoryPositions[count++] = Vector2(320.0f + 80.0f * columns, 400.0f + 80.0f * rows);  // top left Right and down
		}
	}

	for (int i = 0; i < 9; ++i)
	{
		mItemBarPositions[i] = Vector2(320.0f + 80.0f * i, mItemBarY);
		mItemBarIconID[i] = tm->LoadTexture("../Sprites/SageCraft/GUI/Icon_" + std::to_string(i + 1) + ".png");
	}

	InitializeItemTypeList();
	InitializeCraftingRecipies();

	const auto& worldService = GetWorld().GetService<WorldService>();
	SetCurrentBiome(worldService->mChunkData.at(worldService->KeyGenerator(worldService->mNewWorldCoords)).biomeType);
}

void PlayerUIService::Terminate()
{
	mCraftingRecipies.clear();
	mItemTypesList.clear();
	mItemTextureIds.clear();
	playerInventory = nullptr;
	mCameraService = nullptr;
	mCrossHairID = 0;
	mIconSelectID = 0;
	mIconID = 0;

	for (int i = 0; i < 9; ++i)
	{
		mItemBarIconID[i] = 0;
	}
}

void PlayerUIService::Update(float deltaTime)
{
	auto inputSystem = InputSystem::Get();

	// Update Icon Position on hotbar
	IconHotBarUpdate(inputSystem);

	// Open/Close Debug Menu
	if (inputSystem->IsKeyPressed(KeyCode::F3)) {
		mShowDebug = !mShowDebug;
	}

	// Open / Close UI
	if (inputSystem->IsKeyPressed(KeyCode::I)) {

		mUIScreens == UIScreens::None ? SwitchUIScreen(UIScreens::Inventory) : SwitchUIScreen(UIScreens::None);
	}

	// Discard item in hand
	if (mItemInHand && inputSystem->IsMousePressed(MouseButton::LBUTTON))
	{
		const Vector2Int mouse = { inputSystem->GetMouseScreenX(), inputSystem->GetMouseScreenY() };

		const float rectSize = mIconRect.max.x;
		if (mouse.x > mInventoryPositions[26].x + rectSize || mouse.x < mInventoryPositions[18].x - rectSize)
		{
			mItemInHand = false;
			mInventorySlotInHand.EmptySlot();
		}
	}

	// Update Debug Menu
	if (mShowDebug)
	{
		// FPS Text
		mFpsTime -= deltaTime;
		if (mFpsTime < 0.0f) {
			mFpsTime = mFpsTimer;
			mFPS = static_cast<int>(1.0f / deltaTime);
		}
	}
}

void PlayerUIService::Render()
{
	ShowGameplayHotBar();
	DrawSprite(mCrossHairID, Vector2(640.0f, 360.0f));
	RenderXP();

	const auto& playerControllerComponent = GetWorld().GetGameObject(mPlayerGameObjectHandle)->GetComponent<PlayerControllerComponent>();
	if (playerControllerComponent->IsSurvivalMode())
	{
		RenderHealth();
		RenderArmor();
		RenderHunger();
	}

	if (mShowDebug) {
		ShowDebug();
	}

	RenderUIScreen();

	if (mItemInHand)
	{
		auto inputSystem = InputSystem::Get();
		const Vector2 mousePos = { static_cast<float>(inputSystem->GetMouseScreenX()), static_cast<float>(inputSystem->GetMouseScreenY()) };
		DisplayItemIcon(mInventorySlotInHand.itemType, mInventorySlotInHand.amount, mousePos);
	}

	if (mUIScreens == UIScreens::None)
	{
		DrawSprite(mIconSelectID, mIconSelectLocation);
	}
}

void PlayerUIService::DebugUI()
{
	ImGui::DragFloat("Hp Amount", &mHpBarCurrentSize);
	ImGui::DragFloat("Hunger Amount", &mHungerBarCurrentSize);
	ImGui::DragFloat("XP Amount", &mXPBarCurrentSize);
	ImGui::DragFloat("Armor Amount", &mArmorBarCurrentSize);
	//if (ImGui::DragInt("Armor", &mArmorAmount))
	//{
	//	UpdatePlayerArmor(mArmorAmount);
	//}
}

void PlayerUIService::IconHotBarUpdate(const InputSystem* inputSystem)
{
	const float mouseZ = inputSystem->GetMouseMoveZ();
	if (mMouseZCache != mouseZ)
	{
		int scrollAmt = static_cast<int>(mMouseZCache - mouseZ);
		mMouseZCache = mouseZ;

		UpdateIconLocation(mIconSelected + scrollAmt);
		return;
	}

	if (inputSystem->IsKeyPressed(KeyCode::ONE) || inputSystem->IsKeyPressed(KeyCode::NUMPAD1)) {
		UpdateIconLocation(0);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::TWO) || inputSystem->IsKeyPressed(KeyCode::NUMPAD2)) {
		UpdateIconLocation(1);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::THREE) || inputSystem->IsKeyPressed(KeyCode::NUMPAD3)) {
		UpdateIconLocation(2);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::FOUR) || inputSystem->IsKeyPressed(KeyCode::NUMPAD4)) {
		UpdateIconLocation(3);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::FIVE) || inputSystem->IsKeyPressed(KeyCode::NUMPAD5)) {
		UpdateIconLocation(4);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::SIX) || inputSystem->IsKeyPressed(KeyCode::NUMPAD6)) {
		UpdateIconLocation(5);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::SEVEN) || inputSystem->IsKeyPressed(KeyCode::NUMPAD7)) {
		UpdateIconLocation(6);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::EIGHT) || inputSystem->IsKeyPressed(KeyCode::NUMPAD8)) {
		UpdateIconLocation(7);
	}
	else if (inputSystem->IsKeyPressed(KeyCode::NINE) || inputSystem->IsKeyPressed(KeyCode::NUMPAD9)) {
		UpdateIconLocation(8);
	}
}

bool PlayerUIService::IsMouseHovering(const Vector2Int mousePosition, const Vector2 min, const Vector2 max)
{
	return mousePosition.x >= min.x && mousePosition.x <= max.x && mousePosition.y >= min.y && mousePosition.y <= max.y;
}

void PlayerUIService::InventoryOnCreativeLeftClick(int index)
{
	mItemInHand = true;
	mSlotIndex = -1;
	mInventorySlotInHand.itemType = mItemTypesList[index];
	mInventorySlotInHand.amount = 64;
}

void PlayerUIService::DisplayItemIcon(ItemTypes itemType, int amount, Vector2 position)
{
	if (mItemTextureIds.find(itemType) != mItemTextureIds.end())
	{
		DrawSprite(mItemTextureIds.at(itemType), position);
		if (amount >= 0)
		{
			const std::string amountText = std::to_string(amount);
			Font::Get()->Draw(amountText.c_str(), position + 15.0f, 15.0f, Colors::Cyan);
		}
	}
}

void PlayerUIService::UpdateIconLocation(int index)
{
	while (index > 8) { index -= 9; }
	while (index < 0) { index += 9; }

	mIconSelected = index;
	mIconSelectLocation.x = mIconStartLocation.x + mIconOffSet * mIconSelected;
}

void PlayerUIService::ShowDebug()
{
	const auto& cam = mCameraService->GetCamera();

	const auto font = Font::Get();
	font->Draw(mVersionText, 5.0f, 5.0f, 25.0f, Colors::Black);

	// FPS
	{
		const std::string fpsString = std::to_string(mFPS) + " FPS";
		font->Draw(fpsString.c_str(), 5.0f, 30.0f, mFontSize, mFontColor);
	}

	// Position
	{
		std::string position = "XYZ: " + std::to_string(static_cast<int>(cam.GetPosition().x))
			+ ", " + std::to_string(static_cast<int>(cam.GetPosition().y))
			+ ", " + std::to_string(static_cast<int>(cam.GetPosition().z));
		font->Draw(position.c_str(), 5.0f, 80.0f, mFontSize, mFontColor);
	}

	// Chunk Coords
	{
		const std::string chunkString = "Chunk: " + std::to_string(mChunkCoord.x) + ", " + std::to_string(mChunkCoord.y);
		font->Draw(chunkString.c_str(), 5.0f, 105.0f, mFontSize, mFontColor);
	}

	// Facing Angle
	{
		float dirAngle = std::atan2f(cam.GetDirection().z, cam.GetDirection().x) * Constants::RadToDeg - 45.0f;
		if (dirAngle < 0.0f) { dirAngle += 360.0f; }

		if (dirAngle >= 0.0f && dirAngle < 90.0f) {
			font->Draw("Facing: North", 5.0f, 155.0f, mFontSize, mFontColor);
		}
		else if (dirAngle >= 90.0f && dirAngle < 180.0f) {
			font->Draw("Facing: West", 5.0f, 155.0f, mFontSize, mFontColor);
		}
		else if (dirAngle >= 180.0f && dirAngle < 270.0f) {
			font->Draw("Facing: South", 5.0f, 155.0f, mFontSize, mFontColor);
		}
		else {
			font->Draw("Facing: East", 5.0f, 155.0f, mFontSize, mFontColor);
		}
	}

	// Biome
	{
		const std::string chunkString = "Biome: " + BiomeTypeToString(mCurrentBiome);
		font->Draw(chunkString.c_str(), 5.0f, 130.0f, mFontSize, mFontColor);
	}
}

void PlayerUIService::InitializeItemTypeList()
{
	mItemTypesList.push_back(ItemTypes::BedRock);
	mItemTypesList.push_back(ItemTypes::GrassBlock);
	mItemTypesList.push_back(ItemTypes::Dirt);
	mItemTypesList.push_back(ItemTypes::CobbleStone);
	mItemTypesList.push_back(ItemTypes::Stone);
	mItemTypesList.push_back(ItemTypes::OakLeaves);
	mItemTypesList.push_back(ItemTypes::OakLog);
	mItemTypesList.push_back(ItemTypes::OakPlank);
	mItemTypesList.push_back(ItemTypes::Glass);
	mItemTypesList.push_back(ItemTypes::Sand);
	mItemTypesList.push_back(ItemTypes::Gravel);
	mItemTypesList.push_back(ItemTypes::CoalOre);
	mItemTypesList.push_back(ItemTypes::IronOre);
	mItemTypesList.push_back(ItemTypes::GoldOre);
	mItemTypesList.push_back(ItemTypes::DiamondOre);
	mItemTypesList.push_back(ItemTypes::RedStoneOre);
	mItemTypesList.push_back(ItemTypes::LapisOre);
	mItemTypesList.push_back(ItemTypes::EmeraldOre);
	mItemTypesList.push_back(ItemTypes::SnowGrassBlock);
	mItemTypesList.push_back(ItemTypes::Snow);
	mItemTypesList.push_back(ItemTypes::Cactus);
	mItemTypesList.push_back(ItemTypes::CraftingTable);
	mItemTypesList.push_back(ItemTypes::Furnace);
	mItemTypesList.push_back(ItemTypes::OakSlab);
	mItemTypesList.push_back(ItemTypes::CobbleStoneSlab);
	mItemTypesList.push_back(ItemTypes::StoneSlab);
	mItemTypesList.push_back(ItemTypes::Grass);
	mItemTypesList.push_back(ItemTypes::Poppy);
	mItemTypesList.push_back(ItemTypes::Dandelion);
	mItemTypesList.push_back(ItemTypes::SugarCane);
	mItemTypesList.push_back(ItemTypes::Lava);
	mItemTypesList.push_back(ItemTypes::Water);
	mItemTypesList.push_back(ItemTypes::Companion);
	mItemTypesList.push_back(ItemTypes::Stick);
	mItemTypesList.push_back(ItemTypes::Seeds);
	mItemTypesList.push_back(ItemTypes::Sugar);
	mItemTypesList.push_back(ItemTypes::RedDye);
	mItemTypesList.push_back(ItemTypes::YellowDye);
	mItemTypesList.push_back(ItemTypes::GreenDye);
	mItemTypesList.push_back(ItemTypes::Coal);
	mItemTypesList.push_back(ItemTypes::Charcoal);
	mItemTypesList.push_back(ItemTypes::IronIngot);
	mItemTypesList.push_back(ItemTypes::GoldIngot);
	mItemTypesList.push_back(ItemTypes::Diamond);
	mItemTypesList.push_back(ItemTypes::RedStoneDust);
	mItemTypesList.push_back(ItemTypes::Emerald);
	mItemTypesList.push_back(ItemTypes::WoodAxe);
	mItemTypesList.push_back(ItemTypes::WoodHoe);
	mItemTypesList.push_back(ItemTypes::WoodPick);
	mItemTypesList.push_back(ItemTypes::WoodShovel);
	mItemTypesList.push_back(ItemTypes::WoodSword);
	mItemTypesList.push_back(ItemTypes::StoneAxe);
	mItemTypesList.push_back(ItemTypes::StoneHoe);
	mItemTypesList.push_back(ItemTypes::StonePick);
	mItemTypesList.push_back(ItemTypes::StoneShovel);
	mItemTypesList.push_back(ItemTypes::StoneSword);
	mItemTypesList.push_back(ItemTypes::IronAxe);
	mItemTypesList.push_back(ItemTypes::IronHoe);
	mItemTypesList.push_back(ItemTypes::IronPick);
	mItemTypesList.push_back(ItemTypes::IronShovel);
	mItemTypesList.push_back(ItemTypes::IronSword);
	mItemTypesList.push_back(ItemTypes::GoldAxe);
	mItemTypesList.push_back(ItemTypes::GoldHoe);
	mItemTypesList.push_back(ItemTypes::GoldPick);
	mItemTypesList.push_back(ItemTypes::GoldShovel);
	mItemTypesList.push_back(ItemTypes::GoldSword);
	mItemTypesList.push_back(ItemTypes::DiamondAxe);
	mItemTypesList.push_back(ItemTypes::DiamondHoe);
	mItemTypesList.push_back(ItemTypes::DiamondPick);
	mItemTypesList.push_back(ItemTypes::DiamondShovel);
	mItemTypesList.push_back(ItemTypes::DiamondSword);
	mItemTypesList.push_back(ItemTypes::IronHelmet);
	mItemTypesList.push_back(ItemTypes::IronChestPlate);
	mItemTypesList.push_back(ItemTypes::IronLeggings);
	mItemTypesList.push_back(ItemTypes::IronBoots);
	mItemTypesList.push_back(ItemTypes::GoldHelmet);
	mItemTypesList.push_back(ItemTypes::GoldChestPlate);
	mItemTypesList.push_back(ItemTypes::GoldLeggings);
	mItemTypesList.push_back(ItemTypes::GoldBoots);
	mItemTypesList.push_back(ItemTypes::DiamondHelmet);
	mItemTypesList.push_back(ItemTypes::DiamondChestPlate);
	mItemTypesList.push_back(ItemTypes::DiamondLeggings);
	mItemTypesList.push_back(ItemTypes::DiamondBoots);
	mItemTypesList.push_back(ItemTypes::Wheat);
	mItemTypesList.push_back(ItemTypes::Bread);
	mItemTypesList.push_back(ItemTypes::Apple);

	mCreativeIndexCap = (static_cast<int>(mItemTypesList.size()) / 9) - 2;
}

void PlayerUIService::InitializeItemTextureIds()
{
	auto tm = TextureManager::Get();
	mItemTextureIds.insert(std::make_pair(ItemTypes::BedRock, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/bedrock.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GrassBlock, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/grass_side.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Dirt, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/dirt.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::CobbleStone, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/cobblestone.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Stone, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/stone.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::OakLeaves, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/leafs_oak.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::OakLog, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/log_oak.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::OakPlank, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/planks_oak.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Glass, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/glass.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Sand, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/sand.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Gravel, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/gravel.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::CoalOre, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/coal_ore.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronOre, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/iron_ore.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldOre, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/gold_ore.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondOre, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/diamond_ore.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::RedStoneOre, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/redstone_ore.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::LapisOre, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/lapis_ore.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::EmeraldOre, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/emerald_ore.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::SnowGrassBlock, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/grass_side_snowed.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Snow, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/snow.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Cactus, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/cactus_side.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::CraftingTable, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/crafting_table_front.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Furnace, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/furnace_front_off.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::OakSlab, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/planks_oak_slab.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::CobbleStoneSlab, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/cobblestone_slab.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::StoneSlab, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/stone_slab.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Grass, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/plant_grass.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Poppy, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/poppy.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Dandelion, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/dandelion.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::SugarCane, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/reeds.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Lava, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/bucket_lava.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Water, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/bucket_water.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Companion, tm->LoadTexture("../Sprites/SageCraft/Textures/Block/companion.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Stick, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/stick.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Seeds, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/seeds_wheat.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Sugar, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/sugar.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::RedDye, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/dye_powder_red.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::YellowDye, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/dye_powder_yellow.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GreenDye, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/dye_powder_green.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Coal, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/coal.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Charcoal, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/charcoal.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronIngot, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_ingot.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldIngot, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_ingot.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Diamond, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::RedStoneDust, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/redstone_dust.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Emerald, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/emerald.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::WoodAxe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/wood_axe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::WoodHoe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/wood_hoe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::WoodPick, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/wood_pickaxe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::WoodShovel, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/wood_shovel.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::WoodSword, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/wood_sword.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::StoneAxe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/stone_axe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::StoneHoe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/stone_hoe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::StonePick, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/stone_pickaxe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::StoneShovel, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/stone_shovel.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::StoneSword, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/stone_sword.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronAxe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_axe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronHoe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_hoe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronPick, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_pickaxe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronShovel, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_shovel.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronSword, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_sword.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldAxe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_axe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldHoe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_hoe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldPick, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_pickaxe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldShovel, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_shovel.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldSword, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_sword.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondAxe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_axe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondHoe, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_hoe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondPick, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_pickaxe.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondShovel, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_shovel.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondSword, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_sword.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronBoots, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_boots.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronChestPlate, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_chestplate.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronHelmet, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_helmet.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::IronLeggings, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/iron_leggings.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldBoots, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_boots.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldChestPlate, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_chestplate.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldHelmet, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_helmet.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::GoldLeggings, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/gold_leggings.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondBoots, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_boots.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondChestPlate, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_chestplate.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondHelmet, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_helmet.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::DiamondLeggings, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/diamond_leggings.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Wheat, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/wheat.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Bread, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/bread.png")));
	mItemTextureIds.insert(std::make_pair(ItemTypes::Apple, tm->LoadTexture("../Sprites/SageCraft/Textures/Item/apple.png")));
}

#pragma region ---UIScreenLogic---

void PlayerUIService::ShowSurvialInventory()
{
	auto inputSystem = InputSystem::Get();

	const Vector2 offSet = mIconRect.max * 0.5f;
	const Vector2Int mousePos = { inputSystem->GetMouseScreenX(), inputSystem->GetMouseScreenY() };

	HotBarLogic(mousePos, offSet, inputSystem);

	for (int i = 0; i < 27; ++i)
	{
		const Vector2 pos = mInventoryPositions[i];
		DrawSprite(mIconID, pos);
		const Vector2 min = pos - offSet;
		const Vector2 max = pos + offSet;

		DisplayItemIcon(playerInventory[i + 9].itemType, playerInventory[i + 9].amount, pos);

		if (IsMouseHovering(mousePos, min, max))
		{
			DrawSprite(mIconSelectID, pos);
			if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
			{
				InventoryOnSurvivalLeftClick(i + 9);
			}
			else if (inputSystem->IsMousePressed(MouseButton::RBUTTON))
			{
				InventoryOnSurvivalRightClick(i + 9);
			}
		}
	}

	ShowArmor(mousePos);
}

void PlayerUIService::InventoryOnSurvivalLeftClick(int index)
{
	// No item in hand and index slot
	if (!mItemInHand && playerInventory[index].itemType == ItemTypes::None)
	{
		return;
	}

	if (!mItemInHand && playerInventory[index].itemType != ItemTypes::None)
	{
		// Place item into hand
		mItemInHand = true;
		mSlotIndex = index;
		mInventorySlotInHand = playerInventory[index];
		playerInventory[index].EmptySlot();
	}
	else if (mItemInHand && playerInventory[index].itemType == mInventorySlotInHand.itemType)
	{
		// Put item into slot with the same item. If it goes over 64 then remaining goes into hand
		playerInventory[index].amount += mInventorySlotInHand.amount;
		if (playerInventory[index].amount > 64)
		{
			int amount = playerInventory[index].amount - 64;
			playerInventory[index].amount = 64;
			mInventorySlotInHand.amount = amount;
		}
		else
		{
			mItemInHand = false;
			mInventorySlotInHand.EmptySlot();
		}
	}
	else if (mItemInHand && playerInventory[index].itemType == ItemTypes::None)
	{
		// Place item into that index
		mItemInHand = false;
		playerInventory[index] = mInventorySlotInHand;
		mInventorySlotInHand.EmptySlot();
	}
	else if (mItemInHand && playerInventory[index].itemType != ItemTypes::None)
	{
		// Switch items between hand and inventory
		auto temp = playerInventory[index];
		playerInventory[index] = mInventorySlotInHand;
		mInventorySlotInHand = temp;
	}
}

void PlayerUIService::InventoryOnSurvivalRightClick(int index)
{
	// No item in hand and index slot
	if (!mItemInHand && playerInventory[index].itemType == ItemTypes::None)
	{
		return;
	}

	// TODO: RightClick Pick up. Pick up half the items in a slot

	if (mItemInHand && playerInventory[index].itemType == ItemTypes::None)
	{
		// Place half item into that index
		playerInventory[index].itemType = mInventorySlotInHand.itemType;
		int halfAmount = (mInventorySlotInHand.amount + 1) / 2;
		playerInventory[index].amount = halfAmount;

		mInventorySlotInHand.amount -= halfAmount;
		if (mInventorySlotInHand.amount <= 0)
		{
			mItemInHand = false;
			mInventorySlotInHand.EmptySlot();
		}
	}
	else if (mItemInHand && playerInventory[index].itemType == mInventorySlotInHand.itemType)
	{
		int halfAmount = (mInventorySlotInHand.amount + 1) / 2;
		playerInventory[index].amount += halfAmount;

		if (playerInventory[index].amount > 64)
		{
			halfAmount = playerInventory[index].amount - playerInventory[index].amount - 64;
			playerInventory[index].amount = 64;
		}

		mInventorySlotInHand.amount -= halfAmount;
		if (mInventorySlotInHand.amount <= 0)
		{
			mItemInHand = false;
			mInventorySlotInHand.EmptySlot();
		}
	}
}

void PlayerUIService::ShowCreativeInventory()
{
	auto inputSystem = InputSystem::Get();

	const Vector2 offSet = mIconRect.max * 0.5f;
	const Vector2Int mousePos = { inputSystem->GetMouseScreenX(), inputSystem->GetMouseScreenY() };

	HotBarLogic(mousePos, offSet, inputSystem);

	// Up Arrow
	const Vector2 upArrowPos = mInventoryPositions[8] + Vector2(80.0f, 40.0f);
	const Vector2 upArrowMin = upArrowPos - offSet;
	const Vector2 upArrowMax = upArrowPos + offSet;
	DrawSprite(mIconArrowID, upArrowPos);
	if (IsMouseHovering(mousePos, upArrowMin, upArrowMax))
	{
		DrawSprite(mIconSelectID, upArrowPos);
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
		{
			mCreativeIndex = Clamp(mCreativeIndex - 1, 0, mCreativeIndexCap);
		}
	}

	// Down Arrow
	const Vector2 downArrowPos = mInventoryPositions[8] + Vector2(80.0f, 120.0f);
	const Vector2 downArrowMin = downArrowPos - offSet;
	const Vector2 downArrowMax = downArrowPos + offSet;
	DrawSprite(mIconArrowID, downArrowPos, Constants::Pi);
	if (IsMouseHovering(mousePos, downArrowMin, downArrowMax))
	{
		DrawSprite(mIconSelectID, downArrowPos);
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
		{
			mCreativeIndex = Clamp(mCreativeIndex + 1, 0, mCreativeIndexCap);
		}
	}

	const int inventoryOffSet = 9 * mCreativeIndex;
	for (int i = 0; i < 27; ++i)
	{
		const Vector2 pos = mInventoryPositions[i];
		DrawSprite(mIconID, pos);

		const int index = i + inventoryOffSet;
		if (index < mItemTypesList.size())
		{
			DisplayItemIcon(mItemTypesList[index], 64, pos);

			// Mouse Hover
			if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
			{
				DrawSprite(mIconSelectID, pos);
				if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
				{
					InventoryOnCreativeLeftClick(index);
				}
			}
		}
	}

	ShowArmor(mousePos);
}

void PlayerUIService::ShowInventoryCrafting()
{
	auto inputSystem = InputSystem::Get();
	const Vector2Int mousePos = { inputSystem->GetMouseScreenX(), inputSystem->GetMouseScreenY() };

	const Vector2 offSet = mIconRect.max * 0.5f;
	Vector2 pos;

	// Crafting 
	int craftingSlotCount = 0;
	for (int rows = 0; rows < 2; ++rows)
	{
		for (int columns = 0; columns < 2; ++columns)
		{
			pos = Vector2(720.0f + 80.0f * columns, 260.0f - 80.0f * rows);

			DrawSprite(mIconID, pos);
			DisplayItemIcon(mCraftingSlots[craftingSlotCount].itemType, mCraftingSlots[craftingSlotCount].amount, pos);

			if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
			{
				DrawSprite(mIconSelectID, pos);
				if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
				{
					if (mItemInHand && mCraftingSlots[craftingSlotCount].itemType == ItemTypes::None) //Item in hand and empty slot
					{
						mItemInHand = false;
						mCraftingSlots[craftingSlotCount] = mInventorySlotInHand;
						mInventorySlotInHand.EmptySlot();
					}
					else if (mItemInHand && mCraftingSlots[craftingSlotCount].itemType == mInventorySlotInHand.itemType)
					{
						// Put item into slot with the same item. If it goes over 64 then remaining goes into hand
						mCraftingSlots[craftingSlotCount].amount += mInventorySlotInHand.amount;
						if (mCraftingSlots[craftingSlotCount].amount > 64)
						{
							int amount = mCraftingSlots[craftingSlotCount].amount - 64;
							mCraftingSlots[craftingSlotCount].amount = 64;
							mInventorySlotInHand.amount = amount;
						}
						else
						{
							mItemInHand = false;
							mInventorySlotInHand.EmptySlot();
						}
					}
					else if (mItemInHand && mCraftingSlots[craftingSlotCount].itemType != ItemTypes::None) // Item in hand and full slot
					{
						auto tempInventorySlot = mCraftingSlots[craftingSlotCount];
						mCraftingSlots[craftingSlotCount] = mInventorySlotInHand;
						mInventorySlotInHand = tempInventorySlot;
					}
					else if (!mItemInHand && mCraftingSlots[craftingSlotCount].itemType != ItemTypes::None) // No item in hand but item in slot
					{
						mItemInHand = true;
						mInventorySlotInHand = mCraftingSlots[craftingSlotCount];
						mCraftingSlots[craftingSlotCount].EmptySlot();
					}

					UpdateCraftingProduceSlot();
				}
				else if (inputSystem->IsMousePressed(MouseButton::RBUTTON))
				{
					if (!mItemInHand)
					{
						continue;
					}

					if (mCraftingSlots[craftingSlotCount].itemType != ItemTypes::None && mCraftingSlots[craftingSlotCount].itemType != mInventorySlotInHand.itemType)
					{
						continue;
					}

					if (mCraftingSlots[craftingSlotCount].amount >= 64)
					{
						continue;
					}

					mCraftingSlots[craftingSlotCount].itemType = mInventorySlotInHand.itemType;
					++mCraftingSlots[craftingSlotCount].amount;
					--mInventorySlotInHand.amount;

					if (mInventorySlotInHand.amount <= 0)
					{
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}

					UpdateCraftingProduceSlot();
				}
			}

			if (craftingSlotCount == 0)
			{
				craftingSlotCount = 1;
			}
			else if (craftingSlotCount == 1)
			{
				craftingSlotCount = 3;
			}
			else if (craftingSlotCount == 3)
			{
				craftingSlotCount = 4;
			}
		}
	}

	// Arrow
	DrawSprite(mIconArrowID, { 880.0f, 220.0f }, Constants::HalfPi);

	// Item Produced
	pos = Vector2(960.0f, 220.0f);
	DrawSprite(mIconID, pos);

	if (mProduceSlot.itemType == ItemTypes::None)
	{
		return;
	}

	DisplayItemIcon(mProduceSlot.itemType, mProduceSlot.amount, pos);

	if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
	{
		DrawSprite(mIconSelectID, pos);
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
		{
			// Prevent pick up
			if (mItemInHand && (mInventorySlotInHand.itemType != mProduceSlot.itemType || mInventorySlotInHand.amount + mProduceSlot.amount > 64))
			{
				return;
			}

			// Consume items
			for (int i = 0; i < 9; ++i)
			{
				if (mCraftingSlots[i].itemType != ItemTypes::None)
				{
					if (--mCraftingSlots[i].amount <= 0)
					{
						mCraftingSlots[i].EmptySlot();
					}
				}
			}

			// Give items
			mInventorySlotInHand = InventorySlot(mProduceSlot.itemType, mInventorySlotInHand.amount + mProduceSlot.amount);
			mItemInHand = true;
			mSlotIndex = -1;

			// Update Crafting Produce Slot
			UpdateCraftingProduceSlot();
		}
	}
}

void PlayerUIService::ShowArmor(Vector2Int mousePosition)
{
	auto inputSystem = InputSystem::Get();

	for (int i = 0; i < 4; ++i)
	{
		Vector2 pos = Vector2(320.0f, 80.0f + 80.0f * static_cast<float>(i));
		DrawSprite(mIconID, pos);

		DisplayItemIcon(mArmorSlots[i].itemType, -1, pos);
		
		const Vector2 offSet = mIconRect.max * 0.5f;
		const Vector2 min = pos - offSet;
		const Vector2 max = pos + offSet;

		if (IsMouseHovering(mousePosition, min, max))
		{
			DrawSprite(mIconSelectID, pos);

			if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
			{
				if (i == 0)
				{
					if (mArmorSlots[i].itemType == ItemTypes::None && IsHelmet(mInventorySlotInHand.itemType)) // slot is empty but hand is helmet
					{
						mArmorSlots[i] = mInventorySlotInHand;
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && !mItemInHand) // Slot is full but hand is empty
					{
						mInventorySlotInHand = mArmorSlots[i];
						mItemInHand = true;
						mArmorSlots[i].EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && IsHelmet(mInventorySlotInHand.itemType)) // Slot is full but hand is helmet
					{
						auto tempSlot = mArmorSlots[i];
						mArmorSlots[i] = mInventorySlotInHand;
						mInventorySlotInHand = tempSlot;
					}
				}
				else if (i == 1)
				{
					if (mArmorSlots[i].itemType == ItemTypes::None && IsChestPlate(mInventorySlotInHand.itemType)) // slot is empty but hand is chestplate
					{
						mArmorSlots[i] = mInventorySlotInHand;
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && !mItemInHand) // Slot is full but hand is empty
					{
						mInventorySlotInHand = mArmorSlots[i];
						mItemInHand = true;
						mArmorSlots[i].EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && IsChestPlate(mInventorySlotInHand.itemType)) // Slot is full but hand is chestplate
					{
						auto tempSlot = mArmorSlots[i];
						mArmorSlots[i] = mInventorySlotInHand;
						mInventorySlotInHand = tempSlot;
					}
				}
				else if (i == 2)
				{
					if (mArmorSlots[i].itemType == ItemTypes::None && IsLeggings(mInventorySlotInHand.itemType)) // slot is empty but hand is IsLeggings
					{
						mArmorSlots[i] = mInventorySlotInHand;
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && !mItemInHand) // Slot is full but hand is empty
					{
						mInventorySlotInHand = mArmorSlots[i];
						mItemInHand = true;
						mArmorSlots[i].EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && IsLeggings(mInventorySlotInHand.itemType)) // Slot is full but hand is IsLeggings
					{
						auto tempSlot = mArmorSlots[i];
						mArmorSlots[i] = mInventorySlotInHand;
						mInventorySlotInHand = tempSlot;
					}
				}
				else if (i == 3)
				{
					if (mArmorSlots[i].itemType == ItemTypes::None && IsBoots(mInventorySlotInHand.itemType)) // slot is empty but hand is IsBoots
					{
						mArmorSlots[i] = mInventorySlotInHand;
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && !mItemInHand) // Slot is full but hand is empty
					{
						mInventorySlotInHand = mArmorSlots[i];
						mItemInHand = true;
						mArmorSlots[i].EmptySlot();
					}
					else if (mArmorSlots[i].itemType != ItemTypes::None && IsBoots(mInventorySlotInHand.itemType)) // Slot is full but hand is IsBoots
					{
						auto tempSlot = mArmorSlots[i];
						mArmorSlots[i] = mInventorySlotInHand;
						mInventorySlotInHand = tempSlot;
					}
				}

				UpdatePlayerArmor(ArmorPoints(mArmorSlots[0].itemType) + ArmorPoints(mArmorSlots[1].itemType) + ArmorPoints(mArmorSlots[2].itemType) + ArmorPoints(mArmorSlots[3].itemType));
			}
		}
	}
}

void PlayerUIService::ShowCrafting()
{
	auto inputSystem = InputSystem::Get();
	const Vector2Int mousePos = { inputSystem->GetMouseScreenX(), inputSystem->GetMouseScreenY() };

	const Vector2 offSet = mIconRect.max * 0.5f;
	Vector2 pos;

	// Crafting 
	int craftingSlotCount = 0;
	for (int rows = 0; rows < 3; ++rows)
	{
		for (int columns = 0; columns < 3; ++columns)
		{
			pos = Vector2(640.0f + 80.0f * columns, 300.0f - 80.0f * rows);

			DrawSprite(mIconID, pos);
			DisplayItemIcon(mCraftingSlots[craftingSlotCount].itemType, mCraftingSlots[craftingSlotCount].amount, pos);

			if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
			{
				DrawSprite(mIconSelectID, pos);
				if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
				{
					if (mItemInHand && mCraftingSlots[craftingSlotCount].itemType == ItemTypes::None) //Item in hand and empty slot
					{
						mItemInHand = false;
						mCraftingSlots[craftingSlotCount] = mInventorySlotInHand;
						mInventorySlotInHand.EmptySlot();
					}
					else if (mItemInHand && mCraftingSlots[craftingSlotCount].itemType == mInventorySlotInHand.itemType)
					{
						// Put item into slot with the same item. If it goes over 64 then remaining goes into hand
						mCraftingSlots[craftingSlotCount].amount += mInventorySlotInHand.amount;
						if (mCraftingSlots[craftingSlotCount].amount > 64)
						{
							int amount = mCraftingSlots[craftingSlotCount].amount - 64;
							mCraftingSlots[craftingSlotCount].amount = 64;
							mInventorySlotInHand.amount = amount;
						}
						else
						{
							mItemInHand = false;
							mInventorySlotInHand.EmptySlot();
						}
					}
					else if (mItemInHand && mCraftingSlots[craftingSlotCount].itemType != ItemTypes::None) // Item in hand and full slot
					{
						auto tempInventorySlot = mCraftingSlots[craftingSlotCount];
						mCraftingSlots[craftingSlotCount] = mInventorySlotInHand;
						mInventorySlotInHand = tempInventorySlot;
					}
					else if (!mItemInHand && mCraftingSlots[craftingSlotCount].itemType != ItemTypes::None) // No item in hand but item in slot
					{
						mItemInHand = true;
						mInventorySlotInHand = mCraftingSlots[craftingSlotCount];
						mCraftingSlots[craftingSlotCount].EmptySlot();
					}

					UpdateCraftingProduceSlot();
				}
				else if (inputSystem->IsMousePressed(MouseButton::RBUTTON))
				{
					if (!mItemInHand)
					{
						continue;
					}

					if (mCraftingSlots[craftingSlotCount].itemType != ItemTypes::None && mCraftingSlots[craftingSlotCount].itemType != mInventorySlotInHand.itemType)
					{
						continue;
					}

					if (mCraftingSlots[craftingSlotCount].amount >= 64)
					{
						continue;
					}

					mCraftingSlots[craftingSlotCount].itemType = mInventorySlotInHand.itemType;
					++mCraftingSlots[craftingSlotCount].amount;
					--mInventorySlotInHand.amount;

					if (mInventorySlotInHand.amount <= 0)
					{
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}

					UpdateCraftingProduceSlot();
				}
			}

			++craftingSlotCount;
		}
	}

	// Arrow
	DrawSprite(mIconArrowID, { 880.0f, 220.0f }, Constants::HalfPi);

	// Item Produced
	pos = Vector2(960.0f, 220.0f);
	DrawSprite(mIconID, pos);

	if (mProduceSlot.itemType == ItemTypes::None)
	{
		return;
	}

	DisplayItemIcon(mProduceSlot.itemType, mProduceSlot.amount, pos);

	if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
	{
		DrawSprite(mIconSelectID, pos);
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
		{
			// Prevent pick up
			if (mItemInHand && (mInventorySlotInHand.itemType != mProduceSlot.itemType || mInventorySlotInHand.amount + mProduceSlot.amount > 64))
			{
				return;
			}

			// Consume items
			for (int i = 0; i < 9; ++i)
			{
				if (mCraftingSlots[i].itemType != ItemTypes::None)
				{
					if (--mCraftingSlots[i].amount <= 0)
					{
						mCraftingSlots[i].EmptySlot();
					}
				}
			}

			// Give items
			mInventorySlotInHand = InventorySlot(mProduceSlot.itemType, mInventorySlotInHand.amount + mProduceSlot.amount);
			mItemInHand = true;
			mSlotIndex = -1;

			// Update Crafting Produce Slot
			UpdateCraftingProduceSlot();
		}
	}
}

void PlayerUIService::UpdateCraftingProduceSlot()
{
	std::array<ItemTypes, 9> currentRecipe = { mCraftingSlots[0].itemType, mCraftingSlots[1].itemType, mCraftingSlots[2].itemType, mCraftingSlots[3].itemType, mCraftingSlots[4].itemType, mCraftingSlots[5].itemType, mCraftingSlots[6].itemType, mCraftingSlots[7].itemType, mCraftingSlots[8].itemType };

	if (mCraftingRecipies.find(currentRecipe) != mCraftingRecipies.end())
	{
		const RecipeOutcome& recipeOutcome = mCraftingRecipies.at(currentRecipe);
		mProduceSlot.itemType = recipeOutcome.itemType;
		mProduceSlot.amount = recipeOutcome.amount;
		return;
	}

	mProduceSlot.EmptySlot();
}

void PlayerUIService::UpdateFurnaceProduceSlot()
{
	if (mSmeltingSlot.itemType == ItemTypes::None || mFuelSlot.itemType == ItemTypes::None)
	{
		mProduceSlot.EmptySlot();
		return;
	}

	mProduceSlot.itemType = IsSmeltable(mSmeltingSlot.itemType);
	const int fuelAmount = FuelAmount(mFuelSlot.itemType);
	mProduceSlot.amount = mSmeltingSlot.amount >= fuelAmount ? fuelAmount : mSmeltingSlot.amount;
}

void PlayerUIService::ReturnItemsInHand()
{
	if (mItemInHand)
	{
		mItemInHand = false;
		if (mSlotIndex >= 0) // Put back where it came from
		{
			playerInventory[mSlotIndex] = mInventorySlotInHand;
		}
		else // Find a slot to put it into
		{
			// Fill up existing slots
			for (int i = 0; i < 36; ++i)
			{
				if (playerInventory[i].itemType == mInventorySlotInHand.itemType)
				{
					while (playerInventory[i].amount < 64 && mInventorySlotInHand.amount > 0)
					{
						--mInventorySlotInHand.amount;
						++playerInventory[i].amount;
					}

					if (mInventorySlotInHand.amount <= 0)
					{
						break;
					}
				}
			}

			// Find an empty spot to put it
			if (mInventorySlotInHand.amount > 0)
			{
				for (int i = 0; i < 36; ++i)
				{
					if (playerInventory[i].itemType == ItemTypes::None)
					{
						playerInventory[i] = mInventorySlotInHand;
						break;
					}
				}
			}
		}
		mInventorySlotInHand.EmptySlot();
		mSlotIndex = -1;
	}
}

void PlayerUIService::ReturnItemsInCraft()
{
	for (int i = 0; i < 9; ++i)
	{
		if (mCraftingSlots[i].itemType == ItemTypes::None)
		{
			continue;
		}

		// Fill up existing slots
		for (int j = 0; j < 36; ++j)
		{
			if (playerInventory[j].itemType == mCraftingSlots[i].itemType)
			{
				while (playerInventory[j].amount < 64 && mCraftingSlots[i].amount > 0)
				{
					--mCraftingSlots[i].amount;
					++playerInventory[j].amount;
				}

				if (mCraftingSlots[i].amount <= 0)
				{
					break;
				}
			}
		}

		// Find an empty spot to put it
		if (mCraftingSlots[i].amount > 0)
		{
			for (int j = 0; j < 36; ++j)
			{
				if (playerInventory[j].itemType == ItemTypes::None)
				{
					playerInventory[j] = mCraftingSlots[i];
					break;
				}
			}
		}
		mCraftingSlots[i].EmptySlot();
	}
}

void PlayerUIService::ReturnItemsInSmelt()
{
	if (mSmeltingSlot.itemType != ItemTypes::None)
	{
		// Fill up existing slots
		for (int j = 0; j < 36; ++j)
		{
			if (playerInventory[j].itemType == mSmeltingSlot.itemType)
			{
				while (playerInventory[j].amount < 64 && mSmeltingSlot.amount > 0)
				{
					--mSmeltingSlot.amount;
					++playerInventory[j].amount;
				}

				if (mSmeltingSlot.amount <= 0)
				{
					break;
				}
			}
		}

		// Find an empty spot to put it
		if (mSmeltingSlot.amount > 0)
		{
			for (int j = 0; j < 36; ++j)
			{
				if (playerInventory[j].itemType == ItemTypes::None)
				{
					playerInventory[j] = mSmeltingSlot;
					break;
				}
			}
		}
		mSmeltingSlot.EmptySlot();
	}
}

void PlayerUIService::ReturnItemsInFuel()
{
	if (mFuelSlot.itemType != ItemTypes::None)
	{
		// Fill up existing slots
		for (int j = 0; j < 36; ++j)
		{
			if (playerInventory[j].itemType == mFuelSlot.itemType)
			{
				while (playerInventory[j].amount < 64 && mFuelSlot.amount > 0)
				{
					--mFuelSlot.amount;
					++playerInventory[j].amount;
				}

				if (mFuelSlot.amount <= 0)
				{
					break;
				}
			}
		}

		// Find an empty spot to put it
		if (mFuelSlot.amount > 0)
		{
			for (int j = 0; j < 36; ++j)
			{
				if (playerInventory[j].itemType == ItemTypes::None)
				{
					playerInventory[j] = mFuelSlot;
					break;
				}
			}
		}
		mFuelSlot.EmptySlot();
	}
}

void PlayerUIService::ShowFurnace()
{
	auto inputSystem = InputSystem::Get();
	const Vector2Int mousePos = { inputSystem->GetMouseScreenX(), inputSystem->GetMouseScreenY() };

	const Vector2 offSet = mIconRect.max * 0.5f;
	Vector2 pos;

	// Smelting 
	pos = Vector2(800.0f, 140.0f);
	DrawSprite(mIconID, pos);
	DisplayItemIcon(mSmeltingSlot.itemType, mSmeltingSlot.amount, pos);
	if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
	{
		DrawSprite(mIconSelectID, pos);
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
		{
			const bool isItemInHandSmeltable = IsSmeltable(mInventorySlotInHand.itemType) != ItemTypes::None;
			if (isItemInHandSmeltable && IsSmeltable(mSmeltingSlot.itemType) == ItemTypes::None) // Item in hand and empty fuel slot
			{
				mItemInHand = false;
				mSmeltingSlot = mInventorySlotInHand;
				mInventorySlotInHand.EmptySlot();
			}
			else if (isItemInHandSmeltable && mSmeltingSlot.itemType != mInventorySlotInHand.itemType) // Item in hand but diff smeltable items so swap
			{
				auto tempSmeltingSlot = mSmeltingSlot;
				mSmeltingSlot = mInventorySlotInHand;
				mInventorySlotInHand = tempSmeltingSlot;
			}
			else if (isItemInHandSmeltable && mSmeltingSlot.itemType == mInventorySlotInHand.itemType) // Item in hand and smelting slot has same item in it
			{
				while (mSmeltingSlot.amount < 64 && mInventorySlotInHand.amount > 0)
				{
					++mSmeltingSlot.amount;
					--mInventorySlotInHand.amount;
				}

				if (mInventorySlotInHand.amount <= 0)
				{
					mItemInHand = false;
					mInventorySlotInHand.EmptySlot();
				}
			}
			else if (mInventorySlotInHand.itemType == ItemTypes::None && mSmeltingSlot.itemType != ItemTypes::None)
			{
				mItemInHand = true;
				mInventorySlotInHand = mSmeltingSlot;
				mSmeltingSlot.EmptySlot();
			}

			UpdateFurnaceProduceSlot();
		}
		else if (inputSystem->IsMousePressed(MouseButton::RBUTTON))
		{
			const bool isItemInHandSmeltable = IsSmeltable(mInventorySlotInHand.itemType) != ItemTypes::None;

			if (isItemInHandSmeltable && (mInventorySlotInHand.itemType == mSmeltingSlot.itemType || mSmeltingSlot.itemType == ItemTypes::None))
			{
				if (mSmeltingSlot.amount < 64)
				{
					mSmeltingSlot.itemType = mInventorySlotInHand.itemType;
					++mSmeltingSlot.amount;
					if (--mInventorySlotInHand.amount <= 0)
					{
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}
				}
			}

			UpdateFurnaceProduceSlot();
		}
	}

	// Flame Icon
	DrawSprite(mIconFlameID, Vector2(800.0f, 220.0f));

	// Fuel Slot
	pos = Vector2(800.0f, 300.0f);
	DrawSprite(mIconID, pos);
	DisplayItemIcon(mFuelSlot.itemType, mFuelSlot.amount, pos);
	if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
	{
		DrawSprite(mIconSelectID, pos);
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
		{
			if (IsFuel(mInventorySlotInHand.itemType) && mFuelSlot.itemType == ItemTypes::None) // Item in hand and empty fuel slot
			{
				mItemInHand = false;
				mFuelSlot = mInventorySlotInHand;
				mInventorySlotInHand.EmptySlot();
			}
			else if (IsFuel(mInventorySlotInHand.itemType) && mInventorySlotInHand.itemType != mFuelSlot.itemType) // Item in hand and fuel slot has a diff fuel
			{
				auto tempFuelSlot = mFuelSlot;
				mFuelSlot = mInventorySlotInHand;
				mInventorySlotInHand = tempFuelSlot;
			}
			else if (IsFuel(mInventorySlotInHand.itemType) && mInventorySlotInHand.itemType == mFuelSlot.itemType) // Item in hand and fuel slot has same fuel in it
			{
				while (mFuelSlot.amount < 64 && mInventorySlotInHand.amount > 0)
				{
					++mFuelSlot.amount;
					--mInventorySlotInHand.amount;
				}

				if (mInventorySlotInHand.amount <= 0)
				{
					mItemInHand = false;
					mInventorySlotInHand.EmptySlot();
				}
			}
			else if (mInventorySlotInHand.itemType == ItemTypes::None && mFuelSlot.itemType != ItemTypes::None)
			{
				mItemInHand = true;
				mInventorySlotInHand = mFuelSlot;
				mFuelSlot.EmptySlot();
			}

			UpdateFurnaceProduceSlot();
		}
		else if (inputSystem->IsMousePressed(MouseButton::RBUTTON))
		{
			if (IsFuel(mInventorySlotInHand.itemType) && (mInventorySlotInHand.itemType == mFuelSlot.itemType || mFuelSlot.itemType == ItemTypes::None))
			{
				if (mFuelSlot.amount < 64)
				{
					mFuelSlot.itemType = mInventorySlotInHand.itemType;
					++mFuelSlot.amount;
					if (--mInventorySlotInHand.amount <= 0)
					{
						mItemInHand = false;
						mInventorySlotInHand.EmptySlot();
					}
				}
			}
			UpdateFurnaceProduceSlot();
		}
	}

	// Arrow
	DrawSprite(mIconArrowID, { 880.0f, 220.0f }, Constants::HalfPi);

	// Item Produced
	pos = Vector2(960.0f, 220.0f);
	DrawSprite(mIconID, pos);
	if (mProduceSlot.itemType == ItemTypes::None)
	{
		return;
	}

	DisplayItemIcon(mProduceSlot.itemType, mProduceSlot.amount, pos);
	if (IsMouseHovering(mousePos, pos - offSet, pos + offSet))
	{
		DrawSprite(mIconSelectID, pos);
		if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
		{
			// Prevent pick up
			if (mItemInHand && (mInventorySlotInHand.itemType != mProduceSlot.itemType || mInventorySlotInHand.amount + mProduceSlot.amount > 64))
			{
				return;
			}

			// Consume items
			if (--mFuelSlot.amount <= 0)
			{
				mFuelSlot.EmptySlot();
			}

			// Give items
			mItemInHand = true;
			mInventorySlotInHand.itemType = mProduceSlot.itemType;
			for (int i = 0; i < mProduceSlot.amount; ++i)
			{
				--mSmeltingSlot.amount;
				++mInventorySlotInHand.amount;
				if (mInventorySlotInHand.amount >= 64 || mSmeltingSlot.amount <= 0)
				{
					break;
				}
			}

			if (mSmeltingSlot.amount <= 0)
			{
				mSmeltingSlot.EmptySlot();
			}

			// Update Frunace Produce Slot
			UpdateFurnaceProduceSlot();
		}
	}
}

void PlayerUIService::InitializeCraftingRecipies()
{
	FILE* file = nullptr;
	std::filesystem::path templateFile = "Crafting_Recipies.json";
	auto err = fopen_s(&file, templateFile.u8string().c_str(), "r");
	ASSERT(err == 0 && file != nullptr, "PlayerUIService --- Failed to open crafting recipies file '%s'", templateFile.u8string().c_str());

	char readBuffer[65536];
	rj::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));

	fclose(file);

	rj::Document document;
	document.ParseStream(readStream);
	auto craftingRecipies = document["CraftingRecipies"].GetObj();

	std::array<ItemTypes, 9> recipe;
	auto GetCraftingRecipie = [&](ItemTypes itemType, const auto& craftingRecipie)
	{
		const char* recipieName = craftingRecipie.name.GetString();

		if (strcmp(recipieName, ItemTypeToString(itemType).c_str()) == 0)
		{
			const int amount = craftingRecipie.value.HasMember("Amount") ? craftingRecipie.value["Amount"].GetInt() : 1;

			if (craftingRecipie.value.HasMember("Recipies"))
			{
				auto recipies = craftingRecipie.value["Recipies"].GetArray();
				for (auto& rec : recipies)
				{
					auto r = rec.GetArray();
					int index = 0;
					for (auto& string : r)
					{
						recipe[index++] = StringToItemType(string.GetString());
					}
					mCraftingRecipies.insert(std::make_pair(recipe, RecipeOutcome(itemType, amount)));
				}
			}
			return true;
		}
		return false;
	};

	for (auto& craftingRecipie : craftingRecipies)
	{
		if (GetCraftingRecipie(ItemTypes::CobbleStoneSlab, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::CraftingTable, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::Furnace, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GreenDye, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::OakPlank, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::OakSlab, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::RedDye, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::Stick, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::StoneSlab, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::Sugar, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::YellowDye, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::WoodAxe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::WoodHoe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::WoodPick, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::WoodShovel, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::WoodSword, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::StoneAxe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::StoneHoe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::StonePick, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::StoneShovel, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::StoneSword, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronAxe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronHoe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronPick, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronShovel, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronSword, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldAxe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldHoe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldPick, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldShovel, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldSword, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondAxe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondHoe, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondPick, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondShovel, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondSword, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronBoots, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronChestPlate, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronHelmet, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::IronLeggings, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldBoots, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldChestPlate, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldHelmet, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::GoldLeggings, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondBoots, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondChestPlate, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondHelmet, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::DiamondLeggings, craftingRecipie)) { continue; }
		if (GetCraftingRecipie(ItemTypes::Bread, craftingRecipie)) { continue; }
	}
}

void PlayerUIService::RenderUIScreen()
{
	switch (mUIScreens)
	{
	case UIScreens::Inventory:
	{
		const auto& playerControllerComponent = GetWorld().GetGameObject(mPlayerGameObjectHandle)->GetComponent<PlayerControllerComponent>();
		playerControllerComponent->IsSurvivalMode() ? ShowSurvialInventory() : ShowCreativeInventory();
		ShowInventoryCrafting();
	}
		break;
	case UIScreens::CraftingTable:
	{
		const auto& playerControllerComponent = GetWorld().GetGameObject(mPlayerGameObjectHandle)->GetComponent<PlayerControllerComponent>();
		playerControllerComponent->IsSurvivalMode() ? ShowSurvialInventory() : ShowCreativeInventory();
		ShowCrafting();
	}
		break;
	case UIScreens::Furnace:
	{
		const auto& playerControllerComponent = GetWorld().GetGameObject(mPlayerGameObjectHandle)->GetComponent<PlayerControllerComponent>();
		playerControllerComponent->IsSurvivalMode() ? ShowSurvialInventory() : ShowCreativeInventory();
		ShowFurnace();
	}
		break;
	}
}

void PlayerUIService::SwitchUIScreen(UIScreens newUIScreen)
{
	if (mUIScreens == newUIScreen)
	{
		return;
	}

	if (mUIScreens != UIScreens::None)
	{
		ExitUIScreen();
	}

	mUIScreens = newUIScreen;
	mUIShowing = mUIScreens != UIScreens::None;
	InputSystem::Get()->ShowSystemCursor(mUIShowing);
}

void PlayerUIService::UpdatePlayerXP(int level, float xpPercentage)
{
	mPlayerLevel = level;
	mXPBarCurrentSize = Lerp(0.0f, mXPBarMaxSize, xpPercentage);
}

void PlayerUIService::UpdatePlayerHunger(float hungerPercentage)
{
	mHungerBarCurrentSize = Lerp(0.0f, mHungerBarMaxSize, hungerPercentage);
}

void PlayerUIService::UpdatePlayerHp(float hpPercentage)
{
	mHpBarCurrentSize = Lerp(0.0f, mHpBarMaxSize, hpPercentage);
}

void PlayerUIService::UpdatePlayerArmor(int armorAmount)
{
	armorAmount = Clamp(armorAmount, 0, 20);
	mArmorBarCurrentSize = Lerp(0.0f, mArmorBarMaxSize, static_cast<float>(armorAmount) * 0.05f);
}

void PlayerUIService::ExitUIScreen()
{
	// Return any items in hand
	ReturnItemsInHand();

	switch (mUIScreens)
	{
	case UIScreens::Inventory:
	case UIScreens::CraftingTable:
		ReturnItemsInCraft();
		break;
	case UIScreens::Furnace:
		ReturnItemsInSmelt();
		ReturnItemsInFuel();
		break;
	}

	// Empty Produce slot
	mProduceSlot.EmptySlot();
}

void PlayerUIService::ShowGameplayHotBar()
{
	for (int i = 0; i < 9; ++i)
	{
		const Vector2 pos = mItemBarPositions[i];
		DrawSprite(mItemBarIconID[i], pos, 0.0f, Pivot::Center, Flip::None);
		DisplayItemIcon(playerInventory[i].itemType, playerInventory[i].amount, pos);
	}
}

void PlayerUIService::HotBarLogic(Vector2Int mousePosition, Vector2 offSet, InputSystem* inputSystem)
{
	for (int i = 0; i < 9; ++i)
	{
		const Vector2 pos = mItemBarPositions[i];
		DisplayItemIcon(playerInventory[i].itemType, playerInventory[i].amount, pos);

		// Mouse Hover
		if (IsMouseHovering(mousePosition, pos - offSet, pos + offSet))
		{
			DrawSprite(mIconSelectID, pos);
			if (inputSystem->IsMousePressed(MouseButton::LBUTTON))
			{
				InventoryOnSurvivalLeftClick(i);
			}
			else if (inputSystem->IsMousePressed(MouseButton::RBUTTON))
			{
				InventoryOnSurvivalRightClick(i);
			}
		}
	}
}

#pragma endregion

void PlayerUIService::RenderHealth()
{
	DrawSprite(mHealthBarEmptyID, Vector2(450.0f, mItemBarY - 80.0f));

	Rect rect;
	rect.max = Vector2(mHpBarCurrentSize, 28.0f);
	DrawSprite(mHealthBarFullID, rect, Vector2(285.0f + mHpBarCurrentSize * 0.5f, mItemBarY - 79.0f));
}

void PlayerUIService::RenderHunger()
{
	DrawSprite(mHungerBarEmptyID, Vector2(840.0f, mItemBarY - 80.0f));

	Rect rect;
	rect.max = Vector2(mHungerBarCurrentSize, 28.0f);
	DrawSprite(mHungerBarFullID, rect, Vector2(690.0f + mHungerBarCurrentSize * 0.5f, mItemBarY - 79.0f));
}

void PlayerUIService::RenderXP()
{
	const std::string level = std::to_string(mPlayerLevel);
	Font::Get()->Draw(level.c_str(), Vector2(640.0f, mItemBarY - 95.0f), 30, Colors::Black);
	DrawSprite(mXpBarEmptyID, Vector2(640.0f, mItemBarY - 50.0f));

	Rect rect;
	rect.max = Vector2(mXPBarCurrentSize, 20.0f);
	DrawSprite(mXpBarFullID, rect, Vector2(284.0f + mXPBarCurrentSize * 0.5f, mItemBarY - 50.0f));
}

void PlayerUIService::RenderArmor()
{
	const float yValue = mItemBarY - 115.0f;
	DrawSprite(mArmorBarEmptyID, Vector2(450.0f, yValue));

	Rect rect;
	rect.max = Vector2(mArmorBarCurrentSize, 32.0f);
	DrawSprite(mArmorBarFullID, rect, Vector2(285.0f + mArmorBarCurrentSize * 0.5f, yValue));
}

