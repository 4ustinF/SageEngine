#pragma once

#include "Enums.h"

class RecipeOutcome
{
public:
	RecipeOutcome(ItemTypes itemType, int amount)
		: itemType(itemType), amount(amount)
	{
	}

	ItemTypes itemType = ItemTypes::None;
	int amount = 0;
};