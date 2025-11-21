// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleGameplayTags.h"


namespace Veggle
{
	namespace Vegetable
	{
		UE_DEFINE_GAMEPLAY_TAG(Tomato,		"Veggle.Vegetable.Tomato");
		UE_DEFINE_GAMEPLAY_TAG(Carrot,		"Veggle.Vegetable.Carrot");
		UE_DEFINE_GAMEPLAY_TAG(Potato,		"Veggle.Vegetable.Potato");
		UE_DEFINE_GAMEPLAY_TAG(Corn,		"Veggle.Vegetable.Corn");
		UE_DEFINE_GAMEPLAY_TAG(Onion,		"Veggle.Vegetable.Onion");
	}
	
	namespace Flavour
	{
		UE_DEFINE_GAMEPLAY_TAG(Butter,			"Veggle.Flavour.Butter");
		UE_DEFINE_GAMEPLAY_TAG(Salt,			"Veggle.Flavour.Salt");
		UE_DEFINE_GAMEPLAY_TAG(Pepper,			"Veggle.Flavour.Pepper");
		UE_DEFINE_GAMEPLAY_TAG(Heat,			"Veggle.Flavour.Heat");
		UE_DEFINE_GAMEPLAY_TAG(WildcardFlavour,	"Veggle.Flavour.WildcardFlavour");
	}

	namespace EventContext
	{
		UE_DEFINE_GAMEPLAY_TAG(Positive,	"Veggle.EventContext.Positive");
		UE_DEFINE_GAMEPLAY_TAG(Negative,	"Veggle.EventContext.Negative");
		UE_DEFINE_GAMEPLAY_TAG(Neutral,		"Veggle.EventContext.Neutral");
		UE_DEFINE_GAMEPLAY_TAG(VeryBad,		"Veggle.EventContext.VeryBad");
	}
	
}
