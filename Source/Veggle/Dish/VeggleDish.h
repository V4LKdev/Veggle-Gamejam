// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "VeggleDish.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
struct FGameplayEffectContextHandle;
class UVeggleDishAttributeSet;
class UVeggleRecipeData;
class AVeggleDish;

UENUM(BlueprintType)
enum class EDishCompletionState : uint8
{
	Success,
	TimeOut
};

DECLARE_DELEGATE_TwoParams(FOnDishCompleted, AVeggleDish* Dish, EDishCompletionState CompletionState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDishServed);


UCLASS()
class VEGGLE_API AVeggleDish : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AVeggleDish();

	void SetRecipe(UVeggleRecipeData* InRecipe) { Recipe = InRecipe; }
	UFUNCTION(BlueprintPure, Category = "Dish")
	UVeggleRecipeData* GetRecipe() const { return Recipe; }
	int32 GetCurrentIngredientCount(const FGameplayTag& IngredientTag) const;
	float GetCurrentAccumulatedScore() const;
	float CalculateFinalScore(bool& OutIsPerfect) const;
	void GetFlavorMatchResult(float& OutMultiplier, bool& bOutIsPerfect) const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Dish")
	void ServeDish();

	/** IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FOnDishCompleted OnDishCompleted;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Dish")
	FOnDishServed OnDishServed;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnVeggieDataApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Context);
	UFUNCTION(BlueprintImplementableEvent, Category = "Dish")
	void SpawnVeggieProxy(const FGameplayTag& InVeggieTag, const bool bWasCorrect);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dish", meta = (AllowPrivateAccess = "true"))
	float PerfectDishBonusMultiplier = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dish", meta = (AllowPrivateAccess = "true"))
	float TimeBonusPerSec = 10.f;

private:
	TMap<FGameplayTag, float> GetCurrentAccumulatedFlavours() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dish", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PlateMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Dish")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, Category = "Dish")
	TObjectPtr<UVeggleDishAttributeSet> AttributeSet;
	UPROPERTY()
	TObjectPtr<UVeggleRecipeData> Recipe;
	UPROPERTY()
	TMap<FGameplayTag, int32> CurrentIngredients;
};
