// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleDishAttributeSet.h"

UVeggleDishAttributeSet::UVeggleDishAttributeSet()
{
	// Initialize default values for attributes
	InitAccumulatedScore(0.0f);

	InitAccumulatedButter(0.0f);
	InitAccumulatedSalt(0.0f);
	InitAccumulatedPepper(0.0f);
	InitAccumulatedHeat(0.0f);
	
	InitCorrectIngredients(0.0f);
	InitIncorrectIngredients(0.0f);
}
