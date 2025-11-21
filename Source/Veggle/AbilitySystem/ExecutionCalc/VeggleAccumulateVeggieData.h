// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Veggle/Dish/VeggleDish.h"
#include "VeggleAccumulateVeggieData.generated.h"


struct FVeggleGameplayEffectContext;
/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleAccumulateVeggieData : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UVeggleAccumulateVeggieData();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	float WrongIngredientPenaltyMultiplier = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Execution")
	float NoHitPenaltyPercentage = 10.f;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	static float GetHitCount(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                           FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	                           UAbilitySystemComponent* TargetASC,
	                           const AVeggleDish* TargetDish,
	                           FVeggleGameplayEffectContext* CustomContext);
	static FGameplayTag GetIngredientTag(const FGameplayEffectSpec& Spec);
	void HandleCorrectIngredient(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	                             UAbilitySystemComponent* TargetASC, FVeggleGameplayEffectContext* CustomContext,
	                             const FGameplayEffectSpec& Spec,
	                             float HitCount) const;
	void HandleIncorrectIngredient(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
								   FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
								   UAbilitySystemComponent* TargetASC, FVeggleGameplayEffectContext* CustomContext,
								   const FGameplayEffectSpec& Spec,
								   float HitCount) const;
	static float CalcFinalBallScore(const FGameplayEffectCustomExecutionParameters& ExecutionParams, float HitCount, bool bWasRightIngredient);
	
};
