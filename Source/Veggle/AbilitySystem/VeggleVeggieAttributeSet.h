// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "VeggleVeggieAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedDelegate, const FGameplayTag&, AttributeTag, float, NewValue);

/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleVeggieAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UVeggleVeggieAttributeSet();
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	FOnAttributeChangedDelegate OnAttributeChanged;
	
	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData HitCount;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, HitCount)
	
	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData SpecialMultiplier;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, SpecialMultiplier)
	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData FillerMultiplier;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, FillerMultiplier)
	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData BoardClearedBonusMultiplier;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, BoardClearedBonusMultiplier)
	
	
	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData Butter;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, Butter)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData Salt;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, Salt)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData Pepper;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, Pepper)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData Heat;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, Heat)

	UPROPERTY(BlueprintReadOnly, Category = "VeggleBall")
	FGameplayAttributeData WildcardFlavour;
	ATTRIBUTE_ACCESSORS(UVeggleVeggieAttributeSet, WildcardFlavour)
};
