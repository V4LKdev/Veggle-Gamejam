// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleVeggieAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Veggle/VeggleGameplayTags.h"
#include "Veggle/Dish/VeggleRecipeData.h"
#include "Veggle/Pegboard/Peg/VegglePegData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleVeggieAttributeSet)

UVeggleVeggieAttributeSet::UVeggleVeggieAttributeSet()
{
    // Initialize default values for attributes
    InitHitCount(0.0f);
    InitSpecialMultiplier(1.0f);
    InitFillerMultiplier(1.0f);
    InitBoardClearedBonusMultiplier(1.0f);

    InitButter(0.0f);
    InitSalt(0.0f);
    InitHeat(0.0f);
    InitPepper(0.0f);
    InitWildcardFlavour(0.0f);
}

// TODO: Refactor
void UVeggleVeggieAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    FGameplayAttribute ChangedAttribute = Attribute;
    FGameplayTag AttributeTag;
    
    if (ChangedAttribute == GetSaltAttribute())
    {
        AttributeTag = Veggle::Flavour::Salt;
    }
    else if (ChangedAttribute == GetHeatAttribute())
    {
        AttributeTag = Veggle::Flavour::Heat;
    }
    else if (ChangedAttribute == GetButterAttribute())
    {
        AttributeTag = Veggle::Flavour::Butter;
    }
    else if (ChangedAttribute == GetPepperAttribute())
    {
        AttributeTag = Veggle::Flavour::Pepper;
    }
    else if (ChangedAttribute == GetWildcardFlavourAttribute())
    {
        AttributeTag = Veggle::Flavour::WildcardFlavour;
    }
    
    if (AttributeTag.IsValid() && OnAttributeChanged.IsBound())
    {
        OnAttributeChanged.Broadcast(AttributeTag, NewValue);
    }
}
