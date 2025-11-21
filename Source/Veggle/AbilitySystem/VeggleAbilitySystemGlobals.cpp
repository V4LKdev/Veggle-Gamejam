// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleAbilitySystemGlobals.h"
#include "VeggleGameplayEffectContext.h"

FGameplayEffectContext* UVeggleAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FVeggleGameplayEffectContext();
}
