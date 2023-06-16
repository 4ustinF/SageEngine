#pragma once

enum class BlockTypes : uint8_t
{
	Air,
	BedRock,
	GrassBlock,
	Dirt,
	CobbleStone,
	Stone,
	OakLeaves,
	OakLog,
	OakPlank,
	Glass,
	Sand,
	Gravel,
	CoalOre,
	IronOre,
	GoldOre,
	DiamondOre,
	RedStoneOre,
	LapisOre,
	EmeraldOre,
	SnowGrassBlock,
	Snow,
	Cactus,

	CraftingTable,
	Furnace,

	OakSlab,
	CobbleStoneSlab,
	StoneSlab,

	Grass,
	Poppy,
	Dandelion,
	SugarCane,

	Lava,
	Water,
	Companion,

	Farmland,
	Wheat,

	Count,
};

enum class ItemTypes : uint8_t
{
	None,
	BedRock,
	GrassBlock,
	Dirt,
	CobbleStone,
	Stone,
	OakLeaves,
	OakLog,
	OakPlank,
	Glass,
	Sand,
	Gravel,
	CoalOre,
	IronOre,
	GoldOre,
	DiamondOre,
	RedStoneOre,
	LapisOre,
	EmeraldOre,
	SnowGrassBlock,
	Snow,
	Cactus,

	CraftingTable,
	Furnace,

	OakSlab,
	CobbleStoneSlab,
	StoneSlab,

	Grass,
	Poppy,
	Dandelion,
	SugarCane,

	Lava,
	Water,
	Companion,

	Stick,
	Seeds,
	Sugar,
	RedDye,
	YellowDye,
	GreenDye,
	Coal,
	Charcoal,
	IronIngot,
	GoldIngot,
	Diamond,
	RedStoneDust,
	Emerald,
	WoodAxe,
	WoodHoe,
	WoodPick,
	WoodShovel,
	WoodSword,
	StoneAxe,
	StoneHoe,
	StonePick,
	StoneShovel,
	StoneSword,
	IronAxe,
	IronHoe,
	IronPick,
	IronShovel,
	IronSword,
	GoldAxe,
	GoldHoe,
	GoldPick,
	GoldShovel,
	GoldSword,
	DiamondAxe,
	DiamondHoe,
	DiamondPick,
	DiamondShovel,
	DiamondSword,
	IronBoots,
	IronChestPlate,
	IronHelmet,
	IronLeggings,
	GoldBoots,
	GoldChestPlate,
	GoldHelmet,
	GoldLeggings,
	DiamondBoots,
	DiamondChestPlate,
	DiamondHelmet,
	DiamondLeggings,

	Wheat,
	Bread,
	Apple,

	Count,
};

enum class BiomeTypes
{
	Desert,
	Forest,
	Mountains,
	Ocean,
	Plains,
	SnowyTundra,

	Count,
};

enum class ChunkWall
{
	North,
	East,
	South,
	West
};

enum class CloudType
{
	None,
	Fast,
	Fancy,
};

enum class UIScreens
{
	None,
	Inventory,
	CraftingTable,
	Furnace
};

enum class DirectionVector : uint8_t
{
	North,
	East,
	South, 
	West
};

enum class AnimalType
{
	Chicken,
	Cow,
	Pig,
	Sheep
};

namespace
{
	const std::string BlockTypeToString(BlockTypes blockType)
	{
		switch (blockType)
		{
		case BlockTypes::Air: return "Air";
		case BlockTypes::BedRock: return "BedRock";
		case BlockTypes::GrassBlock: return "Grass Block";
		case BlockTypes::Dirt: return "Dirt";
		case BlockTypes::CobbleStone: return "CobbleStone";
		case BlockTypes::Stone: return "Stone";
		case BlockTypes::OakLeaves: return "Oak Leaves";
		case BlockTypes::OakLog: return "Oak Log";
		case BlockTypes::OakPlank: return "Oak Plank";
		case BlockTypes::Glass: return "Glass";
		case BlockTypes::Sand: return "Sand";
		case BlockTypes::Gravel: return "Gravel";
		case BlockTypes::CoalOre: return "Coal Ore";
		case BlockTypes::IronOre: return "Iron Ore";
		case BlockTypes::GoldOre: return "Gold Ore";
		case BlockTypes::DiamondOre: return "Diamond Ore";
		case BlockTypes::RedStoneOre: return "RedStone Ore";
		case BlockTypes::LapisOre: return "Lapis Ore";
		case BlockTypes::EmeraldOre: return "Emerald Ore";
		case BlockTypes::SnowGrassBlock: return "Snow GrassBlock";
		case BlockTypes::Snow: return "Snow";
		case BlockTypes::Cactus: return "Cactus";

		case BlockTypes::CraftingTable: return "Crafting Table";
		case BlockTypes::Furnace: return "Furnace";

		case BlockTypes::OakSlab: return "Oak Slab";
		case BlockTypes::CobbleStoneSlab: return "CobbleStone Slab";
		case BlockTypes::StoneSlab: return "Stone Slab";

		case BlockTypes::Grass: return "Grass";
		case BlockTypes::Poppy: return "Poppy";
		case BlockTypes::Dandelion: return "Dandelion";
		case BlockTypes::SugarCane: return "Sugar Cane";

		case BlockTypes::Lava: return "Lava";
		case BlockTypes::Water: return "Water";
		case BlockTypes::Companion: return "Companion";

		case BlockTypes::Farmland: return "Farmland";
		case BlockTypes::Wheat: return "Wheat";
		default: return "Out of range";
		}
	}

	const std::string ItemTypeToString(ItemTypes itemType)
	{
		switch (itemType)
		{
		case ItemTypes::BedRock: return "BedRock";
		case ItemTypes::GrassBlock: return "Grass Block";
		case ItemTypes::Dirt: return "Dirt";
		case ItemTypes::CobbleStone: return "CobbleStone";
		case ItemTypes::Stone: return "Stone";
		case ItemTypes::OakLeaves: return "Oak Leaves";
		case ItemTypes::OakLog: return "Oak Log";
		case ItemTypes::OakPlank: return "Oak Plank";
		case ItemTypes::Glass: return "Glass";
		case ItemTypes::Sand: return "Sand";
		case ItemTypes::Gravel: return "Gravel";
		case ItemTypes::CoalOre: return "Coal Ore";
		case ItemTypes::IronOre: return "Iron Ore";
		case ItemTypes::GoldOre: return "Gold Ore";
		case ItemTypes::DiamondOre: return "Diamond Ore";
		case ItemTypes::RedStoneOre: return "RedStone Ore";
		case ItemTypes::LapisOre: return "Lapis Ore";
		case ItemTypes::EmeraldOre: return "Emerald Ore";
		case ItemTypes::SnowGrassBlock: return "Snow Grass Block";
		case ItemTypes::Snow: return "Snow";
		case ItemTypes::Cactus: return "Cactus";

		case ItemTypes::CraftingTable: return "Crafting Table";
		case ItemTypes::Furnace: return "Furnace";

		case ItemTypes::OakSlab: return "Oak Slab";
		case ItemTypes::CobbleStoneSlab: return "CobbleStone Slab";
		case ItemTypes::StoneSlab: return "Stone Slab";

		case ItemTypes::Grass: return "Grass";
		case ItemTypes::Poppy: return "Poppy";
		case ItemTypes::Dandelion: return "Dandelion";
		case ItemTypes::SugarCane: return "Sugar Cane";

		case ItemTypes::Lava: return "Lava";
		case ItemTypes::Water: return "Water";
		case ItemTypes::Companion: return "Companion";

		case ItemTypes::Stick: return "Stick";
		case ItemTypes::Seeds: return "Seeds";
		case ItemTypes::Sugar: return "Sugar";
		case ItemTypes::RedDye: return "Red Dye";
		case ItemTypes::YellowDye: return "Yellow Dye";
		case ItemTypes::GreenDye: return "Green Dye";
		case ItemTypes::Coal: return "Coal";
		case ItemTypes::Charcoal: return "Charcoal";
		case ItemTypes::IronIngot: return "Iron Ingot";
		case ItemTypes::GoldIngot: return "Gold Ingot";
		case ItemTypes::Diamond: return "Diamond";
		case ItemTypes::RedStoneDust: return "Red Stone Dust";
		case ItemTypes::Emerald: return "Emerald";

		case ItemTypes::WoodAxe: return "Wood Axe";
		case ItemTypes::WoodHoe: return "Wood Hoe";
		case ItemTypes::WoodPick: return "Wood Pick Axe";
		case ItemTypes::WoodShovel: return "Wood Shovel";
		case ItemTypes::WoodSword: return "Wood Sword";
		case ItemTypes::StoneAxe: return "Stone Axe";
		case ItemTypes::StoneHoe: return "Stone Hoe";
		case ItemTypes::StonePick: return "Stone Pick Axe";
		case ItemTypes::StoneShovel: return "Stone Shovel";
		case ItemTypes::StoneSword: return "Stone Sword";
		case ItemTypes::IronAxe: return "Iron Axe";
		case ItemTypes::IronHoe: return "Iron Hoe";
		case ItemTypes::IronPick: return "Iron Pick Axe";
		case ItemTypes::IronShovel: return "Iron Shovel";
		case ItemTypes::IronSword: return "Iron Sword";
		case ItemTypes::GoldAxe: return "Gold Axe";
		case ItemTypes::GoldHoe: return "Gold Hoe";
		case ItemTypes::GoldPick: return "Gold Pick Axe";
		case ItemTypes::GoldShovel: return "Gold Shovel";
		case ItemTypes::GoldSword: return "Gold Sword";
		case ItemTypes::DiamondAxe: return "Diamond Axe";
		case ItemTypes::DiamondHoe: return "Diamond Hoe";
		case ItemTypes::DiamondPick: return "Diamond Pick Axe";
		case ItemTypes::DiamondShovel: return "Diamond Shovel";
		case ItemTypes::DiamondSword: return "Diamond Sword";
		case ItemTypes::IronBoots: return "Iron Boots";
		case ItemTypes::IronChestPlate: return "Iron ChestPlate";
		case ItemTypes::IronHelmet: return "Iron Helmet";
		case ItemTypes::IronLeggings: return "Iron Leggings";
		case ItemTypes::GoldBoots: return "Gold Boots";
		case ItemTypes::GoldChestPlate: return "Gold ChestPlate";
		case ItemTypes::GoldHelmet: return "Gold Helmet";
		case ItemTypes::GoldLeggings: return "Gold Leggings";
		case ItemTypes::DiamondBoots: return "Diamond Boots";
		case ItemTypes::DiamondChestPlate: return "Diamond ChestPlate";
		case ItemTypes::DiamondHelmet: return "Diamond Helmet";
		case ItemTypes::DiamondLeggings: return "Diamond Leggings";
		case ItemTypes::Wheat: return "Wheat";
		case ItemTypes::Bread: return "Bread";
		case ItemTypes::Apple: return "Apple";
		}

		return "Dirt";
	}

	const ItemTypes StringToItemType(std::string itemString)
	{
		if (itemString == "BedRock") { return ItemTypes::BedRock; }
		else if (itemString == "Grass Block") { return ItemTypes::GrassBlock; }
		else if (itemString == "Dirt") { return ItemTypes::Dirt; }
		else if (itemString == "CobbleStone") { return ItemTypes::CobbleStone; }
		else if (itemString == "Stone") { return ItemTypes::Stone; }
		else if (itemString == "Oak Leaves") { return ItemTypes::OakLeaves; }
		else if (itemString == "Oak Log") { return ItemTypes::OakLog; }
		else if (itemString == "Oak Plank") { return ItemTypes::OakPlank; }
		else if (itemString == "Glass") { return ItemTypes::Glass; }
		else if (itemString == "Sand") { return ItemTypes::Sand; }
		else if (itemString == "Gravel") { return ItemTypes::Gravel; }
		else if (itemString == "Coal Ore") { return ItemTypes::CoalOre; }
		else if (itemString == "Iron Ore") { return ItemTypes::IronOre; }
		else if (itemString == "Gold Ore") { return ItemTypes::GoldOre; }
		else if (itemString == "Lapis Ore") { return ItemTypes::LapisOre; }
		else if (itemString == "SnowGrass Block") { return ItemTypes::SnowGrassBlock; }
		else if (itemString == "Snow") { return ItemTypes::Snow; }
		else if (itemString == "Cactus") { return ItemTypes::Cactus; }
		else if (itemString == "Crafting Table") { return ItemTypes::CraftingTable; }
		else if (itemString == "Furnace") { return ItemTypes::Furnace; }
		else if (itemString == "Oak Slab") { return ItemTypes::OakSlab; }
		else if (itemString == "CobbleStone Slab") { return ItemTypes::CobbleStoneSlab; }
		else if (itemString == "Stone Slab") { return ItemTypes::StoneSlab; }
		else if (itemString == "Grass") { return ItemTypes::Grass; }
		else if (itemString == "Poppy") { return ItemTypes::Poppy; }
		else if (itemString == "Dandelion") { return ItemTypes::Dandelion; }
		else if (itemString == "Sugar Cane") { return ItemTypes::SugarCane; }
		else if (itemString == "Lava") { return ItemTypes::Lava; }
		else if (itemString == "Water") { return ItemTypes::Water; }
		else if (itemString == "Companion") { return ItemTypes::Companion; }
		else if (itemString == "Stick") { return ItemTypes::Stick; }
		else if (itemString == "Seeds") { return ItemTypes::Seeds; }
		else if (itemString == "Sugar") { return ItemTypes::Sugar; }
		else if (itemString == "Red Dye") { return ItemTypes::RedDye; }
		else if (itemString == "Yellow Dye") { return ItemTypes::YellowDye; }
		else if (itemString == "Green Dye") { return ItemTypes::GreenDye; }
		else if (itemString == "Coal") { return ItemTypes::Coal; }
		else if (itemString == "Charcoal") { return ItemTypes::Charcoal; }
		else if (itemString == "Iron Ingot") { return ItemTypes::IronIngot; }
		else if (itemString == "Gold Ingot") { return ItemTypes::GoldIngot; }
		else if (itemString == "Diamond") { return ItemTypes::Diamond; }
		else if (itemString == "Red Stone Dust") { return ItemTypes::RedStoneDust; }
		else if (itemString == "Emerald") { return ItemTypes::Emerald; }
		else if (itemString == "Wood Axe") { return ItemTypes::WoodAxe; }
		else if (itemString == "Wood Hoe") { return ItemTypes::WoodHoe; }
		else if (itemString == "Wood Pick Axe") { return ItemTypes::WoodPick; }
		else if (itemString == "Wood Shovel") { return ItemTypes::WoodShovel; }
		else if (itemString == "Wood Sword") { return ItemTypes::WoodSword; }
		else if (itemString == "Stone Axe") { return ItemTypes::StoneAxe; }
		else if (itemString == "Stone Hoe") { return ItemTypes::StoneHoe; }
		else if (itemString == "Stone Pick Axe") { return ItemTypes::StonePick; }
		else if (itemString == "Stone Shovel") { return ItemTypes::StoneShovel; }
		else if (itemString == "Stone Sword") { return ItemTypes::StoneSword; }
		else if (itemString == "Iron Axe") { return ItemTypes::IronAxe; }
		else if (itemString == "Iron Hoe") { return ItemTypes::IronHoe; }
		else if (itemString == "Iron Pick Axe") { return ItemTypes::IronPick; }
		else if (itemString == "Iron Shovel") { return ItemTypes::IronShovel; }
		else if (itemString == "Iron Sword") { return ItemTypes::IronSword; }
		else if (itemString == "Gold Axe") { return ItemTypes::GoldAxe; }
		else if (itemString == "Gold Hoe") { return ItemTypes::GoldHoe; }
		else if (itemString == "Gold Pick Axe") { return ItemTypes::GoldPick; }
		else if (itemString == "Gold Shovel") { return ItemTypes::GoldShovel; }
		else if (itemString == "Gold Sword") { return ItemTypes::GoldSword; }
		else if (itemString == "Diamond Axe") { return ItemTypes::DiamondAxe; }
		else if (itemString == "Diamond Hoe") { return ItemTypes::DiamondHoe; }
		else if (itemString == "Diamond Pick Axe") { return ItemTypes::DiamondPick; }
		else if (itemString == "Diamond Shovel") { return ItemTypes::DiamondShovel; }
		else if (itemString == "Diamond Sword") { return ItemTypes::DiamondSword; }

		else if (itemString == "Iron Boots") { return ItemTypes::IronBoots; }
		else if (itemString == "Iron ChestPlate") { return ItemTypes::IronChestPlate; }
		else if (itemString == "Iron Helmet") { return ItemTypes::IronHelmet; }
		else if (itemString == "Iron Leggings") { return ItemTypes::IronLeggings; }

		else if (itemString == "Gold Boots") { return ItemTypes::GoldBoots; }
		else if (itemString == "Gold ChestPlate") { return ItemTypes::GoldChestPlate; }
		else if (itemString == "Gold Helmet") { return ItemTypes::GoldHelmet; }
		else if (itemString == "Gold Leggings") { return ItemTypes::GoldLeggings; }

		else if (itemString == "Diamond Boots") { return ItemTypes::DiamondBoots; }
		else if (itemString == "Diamond ChestPlate") { return ItemTypes::DiamondChestPlate; }
		else if (itemString == "Diamond Helmet") { return ItemTypes::DiamondHelmet; }
		else if (itemString == "Diamond Leggings") { return ItemTypes::DiamondLeggings; }

		else if (itemString == "Wheat") { return ItemTypes::Wheat; }
		else if (itemString == "Bread") { return ItemTypes::Bread; }
		else if (itemString == "Apple") { return ItemTypes::Apple; }
		return ItemTypes::None;
	}

	const std::string BiomeTypeToString(BiomeTypes biomeType)
	{
		switch (biomeType)
		{
		case BiomeTypes::Desert: return "Desert";
		case BiomeTypes::Forest: return "Forest";
		case BiomeTypes::Mountains: return "Mountains";
		case BiomeTypes::Ocean: return "Ocean";
		case BiomeTypes::Plains: return "Plains";
		case BiomeTypes::SnowyTundra : return "Snowy Tundra";
		default:
			return "Out of range";
		}
	}

	bool IsItemBlock(ItemTypes itemType)	
	{
		switch (itemType)
		{
		case ItemTypes::BedRock:
		case ItemTypes::GrassBlock:
		case ItemTypes::Dirt:
		case ItemTypes::CobbleStone:
		case ItemTypes::Stone:
		case ItemTypes::OakLeaves:
		case ItemTypes::OakLog:
		case ItemTypes::OakPlank:
		case ItemTypes::Glass:
		case ItemTypes::Sand:
		case ItemTypes::Gravel:
		case ItemTypes::CoalOre:
		case ItemTypes::IronOre:
		case ItemTypes::GoldOre:
		case ItemTypes::DiamondOre:
		case ItemTypes::RedStoneOre:
		case ItemTypes::LapisOre:
		case ItemTypes::EmeraldOre:
		case ItemTypes::SnowGrassBlock:
		case ItemTypes::Snow:
		case ItemTypes::Cactus:
		case ItemTypes::CraftingTable:
		case ItemTypes::Furnace:
		case ItemTypes::Companion:
			return true;
		default:
			return false;
		}
	}

	ItemTypes IsSmeltable(ItemTypes itemType)
	{
		switch (itemType)
		{
		case ItemTypes::OakLog: return ItemTypes::Charcoal;
		case ItemTypes::Sand: return ItemTypes::Glass;
		}

		return ItemTypes::None;
	}

	bool IsFuel(ItemTypes itemType)
	{
		switch (itemType)
		{
		case ItemTypes::Charcoal:
		case ItemTypes::Coal:
		case ItemTypes::CraftingTable:
		case ItemTypes::OakLog:
		case ItemTypes::OakPlank:
		case ItemTypes::OakSlab:
		case ItemTypes::Stick:
			return true;
		}

		return false;
	}

	int FuelAmount(ItemTypes itemType)
	{
		switch (itemType)
		{
		case ItemTypes::Coal:
			return 4;
		case ItemTypes::Charcoal:
			return 3;
		}

		return 1;
	}

	bool IsInstantBreak(BlockTypes blockType)
	{
		switch (blockType)
		{
		case BlockTypes::OakLeaves:
		case BlockTypes::Grass:
		case BlockTypes::Poppy:
		case BlockTypes::Dandelion:
		case BlockTypes::SugarCane:
		case BlockTypes::Wheat:
			return true;
		}
		return false;
	}

	bool IsAxe(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::WoodAxe:
		case ItemTypes::StoneAxe:
		case ItemTypes::IronAxe:
		case ItemTypes::GoldAxe:
		case ItemTypes::DiamondAxe:
			return true;
		default:
			return false;
		}
	}

	bool IsHoe(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::WoodHoe:
		case ItemTypes::StoneHoe:
		case ItemTypes::IronHoe:
		case ItemTypes::GoldHoe:
		case ItemTypes::DiamondHoe:
			return true;
		default:
			return false;
		}
	}

	bool IsPick(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::WoodPick:
		case ItemTypes::StonePick:
		case ItemTypes::IronPick:
		case ItemTypes::GoldPick:
		case ItemTypes::DiamondPick:
			return true;
		default:
			return false;
		}
	}

	bool IsShovel(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::WoodShovel:
		case ItemTypes::StoneShovel:
		case ItemTypes::IronShovel:
		case ItemTypes::GoldShovel:
		case ItemTypes::DiamondShovel:
			return true;
		default:
			return false;
		}
	}

	bool IsSword(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::WoodSword:
		case ItemTypes::StoneSword:
		case ItemTypes::IronSword:
		case ItemTypes::GoldSword:
		case ItemTypes::DiamondSword:
			return true;
		default:
			return false;
		}
	}

	bool IsBoots(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::GoldBoots:
		case ItemTypes::IronBoots:
		case ItemTypes::DiamondBoots:
			return true;
		default:
			return false;
		}
	}

	bool IsLeggings(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::GoldLeggings:
		case ItemTypes::IronLeggings:
		case ItemTypes::DiamondLeggings:
			return true;
		default:
			return false;
		}
	}

	bool IsChestPlate(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::GoldChestPlate:
		case ItemTypes::IronChestPlate:
		case ItemTypes::DiamondChestPlate:
			return true;
		default:
			return false;
		}
	}

	bool IsHelmet(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::GoldHelmet:
		case ItemTypes::IronHelmet:
		case ItemTypes::DiamondHelmet:
			return true;
		default:
			return false;
		}
	}

	int ArmorPoints(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::GoldBoots:
			return 1;
		case ItemTypes::GoldHelmet:
		case ItemTypes::IronHelmet:
		case ItemTypes::IronBoots:
			return 2;
		case ItemTypes::GoldLeggings:
		case ItemTypes::DiamondHelmet:
		case ItemTypes::DiamondBoots:
			return 3;
		case ItemTypes::GoldChestPlate:
		case ItemTypes::IronLeggings:
			return 5;
		case ItemTypes::IronChestPlate:
		case ItemTypes::DiamondLeggings:
			return 6;
		case ItemTypes::DiamondChestPlate:
			return 8;
		default:
			return 0;
		}
	}

	bool IsFood(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::Bread:
		case ItemTypes::Sugar:
		case ItemTypes::Apple:
			return true;
		}
		return false;
	}

	int FoodPoints(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::Sugar:
			return 1;
		case ItemTypes::Apple:
			return 4;
		case ItemTypes::Bread:
			return 5;
		}
		return 0;
	}

	int DamagePoints(const ItemTypes item)
	{
		switch (item)
		{
		case ItemTypes::WoodShovel:
		case ItemTypes::GoldShovel:
			return 2;
		case ItemTypes::WoodHoe:
		case ItemTypes::WoodPick:
		case ItemTypes::StoneShovel:
		case ItemTypes::GoldHoe:
		case ItemTypes::GoldPick:
			return 3;
		case ItemTypes::WoodAxe:
		case ItemTypes::GoldAxe:
		case ItemTypes::StonePick:
		case ItemTypes::StoneHoe:
		case ItemTypes::IronShovel:
			return 4;
		case ItemTypes::WoodSword:
		case ItemTypes::StoneAxe:
		case ItemTypes::GoldSword:
		case ItemTypes::IronHoe:
		case ItemTypes::IronPick:
		case ItemTypes::DiamondShovel:
			return 5;
		case ItemTypes::StoneSword:
		case ItemTypes::IronAxe:
		case ItemTypes::DiamondHoe:
		case ItemTypes::DiamondPick:
			return 6;
		case ItemTypes::IronSword:
		case ItemTypes::DiamondAxe:
			return 7;
		case ItemTypes::DiamondSword:
			return 8;
		}

		return 1;
	}
}
