// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "VeggleDishAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class VEGGLE_API UVeggleDishAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UVeggleDishAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData AccumulatedScore;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, AccumulatedScore)

	
	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData AccumulatedButter;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, AccumulatedButter)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData AccumulatedSalt;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, AccumulatedSalt)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData AccumulatedPepper;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, AccumulatedPepper)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData AccumulatedHeat;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, AccumulatedHeat)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData AccumulatedWildcardFlavour;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, AccumulatedWildcardFlavour)
	
	
	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData CorrectIngredients;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, CorrectIngredients)
	UPROPERTY(BlueprintReadOnly, Category = "VeggleDish")
	FGameplayAttributeData IncorrectIngredients;
	ATTRIBUTE_ACCESSORS(UVeggleDishAttributeSet, IncorrectIngredients)
};
