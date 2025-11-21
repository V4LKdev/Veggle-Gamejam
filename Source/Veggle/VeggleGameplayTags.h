// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace Veggle
{
	namespace Vegetable
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tomato)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Corn)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Carrot)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Potato)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Onion)
	}

	namespace Flavour
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Butter)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Salt)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pepper)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Heat)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(WildcardFlavour)
	}

	namespace EventContext
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Positive)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Negative)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Neutral)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(VeryBad)
	}
}
