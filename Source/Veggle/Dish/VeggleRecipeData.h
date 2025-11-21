// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "VeggleRecipeData.generated.h"

// TODO: Check if needed and move to VeggleGameplayTags.h
namespace Dish
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CorrectIngredients);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(IncorrectIngredients);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(PreparationTime);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(PrepSteps);
}


UENUM(BlueprintType)
enum class EFlavorTargetCondition : uint8
{
	AtLeast UMETA(DisplayName = "At Least"),
	Exactly UMETA(DisplayName = "Exactly"),
	Maximum UMETA(DisplayName = "Maximum")
};

USTRUCT(BlueprintType)
struct FFlavorTarget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flavor Target", meta = (Categories = "Veggle.Flavor"))
	FGameplayTag Flavour;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flavor Target")
	EFlavorTargetCondition Condition;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flavor Target")
	int32 Value;
};

/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleRecipeData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FString RecipeName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	TObjectPtr<UTexture2D> RecipeImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta =(Units = "s"))
	float OptimalPreparationTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (Units = "s"))
	float MaxPreparationTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (Categories = "Veggle.Vegetable"))
	TMap<FGameplayTag, int32> RequiredIngredients;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (Categories = "Veggle.Flavor"))
	TArray<FFlavorTarget> FlavorTargets;
};
