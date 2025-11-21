// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "VeggleGameplayEffectContext.generated.h"

/**
 * 
 */
USTRUCT()
struct FVeggleGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	bool bWasCorrectIngredient = false;
	UPROPERTY()
	bool bWasNoHit = false;

	UPROPERTY()
	FGameplayTag VeggieIngredientTag;
    
	// Required Overrides
	virtual FGameplayEffectContext* Duplicate() const override
	{
		FVeggleGameplayEffectContext* NewContext = new FVeggleGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
};