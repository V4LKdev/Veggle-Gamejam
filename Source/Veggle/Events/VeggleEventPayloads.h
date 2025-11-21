// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VeggleEventPayloads.generated.h"

class UAbilitySystemComponent;
class UVeggleRecipeData;
class UVegglePegData;
class AVeggleDish;
class UVeggleBallData;
class UVeggleWalkingVeggieData;

USTRUCT(BlueprintType)
struct VEGGLE_API FVeggieLoadedPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UVeggleWalkingVeggieData> VeggieData;
};

USTRUCT(BlueprintType)
struct VEGGLE_API FVeggieHitPegPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UVegglePegData> PegType;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> BallASC;
	
	UPROPERTY(BlueprintReadOnly)
	bool bBoardCleared = false;
};

USTRUCT(BlueprintType)
struct VEGGLE_API FVeggieEvaluatedPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bWasCorrectIngredient = false;

	UPROPERTY(BlueprintReadOnly)
	bool bWasNoHit = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDishComplete = false;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag IngredientTag;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AVeggleDish> Dish = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, int32> CurrentIngredients;

	UPROPERTY(BlueprintReadOnly)
	TMap<FGameplayTag, float> AccumulatedFlavours;

	UPROPERTY(BlueprintReadOnly)
	float CurrentScore = 0.0f;
};

USTRUCT(BlueprintType)
struct VEGGLE_API FDishCompletedPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bIsPerfect = false;
	UPROPERTY(BlueprintReadOnly)
	bool bWasTimeOut = false;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UVeggleRecipeData> FinishedRecipe;
};
