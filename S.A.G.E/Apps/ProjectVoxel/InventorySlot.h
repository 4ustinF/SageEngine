#pragma once

#include "Enums.h"

struct InventorySlot
{
	InventorySlot(ItemTypes itemType = ItemTypes::None, int amount = 0)
		:itemType(itemType), amount(amount)
	{
	}

	ItemTypes itemType = ItemTypes::None;
	int amount = 0;

	void EmptySlot()
	{
		itemType = ItemTypes::None;
		amount = 0;
	}
};