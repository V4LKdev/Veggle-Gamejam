// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "VeggleAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;

};
