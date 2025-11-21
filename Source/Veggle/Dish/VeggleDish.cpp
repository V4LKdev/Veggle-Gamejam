// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleDish.h"

#include "AbilitySystemComponent.h"
#include "VeggleRecipeData.h"
#include "Veggle/VeggleGameplayTags.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/VeggleLogChannels.h"
#include "Veggle/AbilitySystem/VeggleDishAttributeSet.h"
#include "Veggle/AbilitySystem/VeggleGameplayEffectContext.h"
#include "Veggle/Core/VeggleEventSubsystem.h"
#include "Veggle/Core/VeggleGameState.h"
#include "Veggle/Events/VeggleEventPayloads.h"


AVeggleDish::AVeggleDish()
{
	PrimaryActorTick.bCanEverTick = false;

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlateMesh");
	SetRootComponent(PlateMesh);
	

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	AttributeSet = CreateDefaultSubobject<UVeggleDishAttributeSet>("DishAttributeSet");
}


int32 AVeggleDish::GetCurrentIngredientCount(const FGameplayTag& IngredientTag) const
{
	if (const int32* Count = CurrentIngredients.Find(IngredientTag))
	{
		return *Count;
	}
	return 0;
}

float AVeggleDish::GetCurrentAccumulatedScore() const
{
	return AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetAccumulatedScoreAttribute());
}

static FGameplayAttribute GetFlavorAttributeByTag(const FGameplayTag& FlavorTag)
{
	if (FlavorTag == Veggle::Flavour::Butter)
		return UVeggleDishAttributeSet::GetAccumulatedButterAttribute();
	if (FlavorTag == Veggle::Flavour::Salt)
		return UVeggleDishAttributeSet::GetAccumulatedSaltAttribute();
	if (FlavorTag == Veggle::Flavour::Pepper)
		return UVeggleDishAttributeSet::GetAccumulatedPepperAttribute();
	if (FlavorTag == Veggle::Flavour::Heat)
		return UVeggleDishAttributeSet::GetAccumulatedHeatAttribute();
	if (FlavorTag == Veggle::Flavour::WildcardFlavour)
		return UVeggleDishAttributeSet::GetAccumulatedWildcardFlavourAttribute();
	return FGameplayAttribute();
}

void AVeggleDish::GetFlavorMatchResult(float& OutMultiplier, bool& bOutIsPerfect) const
{
	TArray<FGameplayTag> AllFlavors = {
		Veggle::Flavour::Butter,
		Veggle::Flavour::Salt,
		Veggle::Flavour::Pepper,
		Veggle::Flavour::Heat,
		Veggle::Flavour::WildcardFlavour
	};

	constexpr int32 CloseTolerance = 1;
	int32 PerfectCount = 0;
	int32 CloseCount = 0;
	int32 OffCount = 0;
	const int32 NumFlavors = AllFlavors.Num();

	for (const FGameplayTag& FlavorTag : AllFlavors)
	{
		const FGameplayAttribute Attr = GetFlavorAttributeByTag(FlavorTag);
		if (!Attr.IsValid())
		{
			PerfectCount++;
			continue;
		}
		const float CurrentValue = AbilitySystemComponent->GetNumericAttribute(Attr);
		const FFlavorTarget* Target = Recipe ? Recipe->FlavorTargets.FindByPredicate([&](const FFlavorTarget& FT) { return FT.Flavour == FlavorTag; }) : nullptr;
		if (!Target)
		{
			PerfectCount++;
			continue;
		}
		bool bPerfect = false;
		bool bClose = false;
		const int32 TargetValue = Target->Value;
		if (Target->Condition == EFlavorTargetCondition::Exactly)
		{
			if (FMath::IsNearlyEqual(CurrentValue, TargetValue, 0.01f))
				bPerfect = true;
			else if (FMath::Abs(CurrentValue - TargetValue) <= CloseTolerance)
				bClose = true;
		}
		else if (Target->Condition == EFlavorTargetCondition::AtLeast)
		{
			if (CurrentValue >= TargetValue)
				bPerfect = true;
			else if (TargetValue - CurrentValue <= CloseTolerance)
				bClose = true;
		}
		else if (Target->Condition == EFlavorTargetCondition::Maximum)
		{
			if (CurrentValue <= TargetValue)
				bPerfect = true;
			else if (CurrentValue - TargetValue <= CloseTolerance)
				bClose = true;
		}
		if (bPerfect)
			PerfectCount++;
		else if (bClose)
			CloseCount++;
		else
			OffCount++;
	}

	if (PerfectCount == NumFlavors)
	{
		OutMultiplier = 3.0f;
		bOutIsPerfect = true;
	}
	else if (PerfectCount + CloseCount == NumFlavors && CloseCount > 0)
	{
		OutMultiplier = 2.0f + 0.5f * static_cast<float>(PerfectCount) / NumFlavors;
		bOutIsPerfect = false;
	}
	else
	{
		OutMultiplier = 1.0f + 0.5f * static_cast<float>(PerfectCount) / NumFlavors;
		bOutIsPerfect = false;
	}

	UE_LOG(LogVeggleCalculation, Log, TEXT("Flavor Match: Perfect=%d, Close=%d, Off=%d, Multiplier=%.2f, IsPerfect=%s"), PerfectCount, CloseCount, OffCount, OutMultiplier, bOutIsPerfect ? TEXT("true") : TEXT("false"));
}

float AVeggleDish::CalculateFinalScore(bool& OutIsPerfect) const
{
	if (!UVeggleGameUtils::GetVeggleGameState(this))
	{
		return -1.0f;
	}
	// Collect Data
	const float TimeRemaining = UVeggleGameUtils::GetVeggleGameState(this)->GetCurrentDishTimeRemaining();
	const float IncorrectIngredients = AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetIncorrectIngredientsAttribute());
	const float BaseScore = AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetAccumulatedScoreAttribute());

	float FinalScore = BaseScore;
	OutIsPerfect = false;

	if (TimeRemaining > 0.f)
	{
		FinalScore += FMath::RoundToInt(TimeRemaining) * TimeBonusPerSec;
		UE_LOG(LogVeggleCalculation, Log, TEXT("Time Bonus: +%.2f (TimeRemaining=%.2f)"), FMath::RoundToInt(TimeRemaining) * TimeBonusPerSec, TimeRemaining);
	}

	float FlavorMultiplier = 1.0f;
	bool bPerfectFlavor = false;
	GetFlavorMatchResult(FlavorMultiplier, bPerfectFlavor);
	FinalScore *= FlavorMultiplier;

	const bool bPerfectTime = (Recipe->MaxPreparationTime - TimeRemaining) <= Recipe->OptimalPreparationTime;
	const bool bPerfectIngredients = IncorrectIngredients <= 0;

	if (bPerfectIngredients && bPerfectTime && bPerfectFlavor)
	{
		FinalScore *= PerfectDishBonusMultiplier;
		OutIsPerfect = true;
		UE_LOG(LogVeggleCalculation, Log, TEXT("Perfect Dish Bonus Applied! Multiplier=%.2f"), PerfectDishBonusMultiplier);
	}
	else
	{
		OutIsPerfect = false;
	}

	UE_LOG(LogVeggleCalculation, Log, TEXT("Final Score: %.2f (Base=%.2f, FlavorMultiplier=%.2f, TimeBonusPerSec=%.2f, PerfectBonus=%.2f, IsPerfect=%s)"), FinalScore, BaseScore, FlavorMultiplier, TimeBonusPerSec, bPerfectIngredients && bPerfectTime ? PerfectDishBonusMultiplier : 1.0f, OutIsPerfect ? TEXT("true") : TEXT("false"));

	return FMath::Max(0.0f, FinalScore);
}

UAbilitySystemComponent* AVeggleDish::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AVeggleDish::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	AbilitySystemComponent->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::OnVeggieDataApplied);

	
}

void AVeggleDish::OnVeggieDataApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Context)
{
	// Some Validity Checks
	if (!AbilitySystemComponent || !Recipe)
	{
		return;
	}

	const FVeggleGameplayEffectContext* CustomContext = static_cast<FVeggleGameplayEffectContext*>(Spec.GetContext().Get());
	if(!CustomContext)
	{
		return;
	}

	// Evaluate the Veggie Data, and Send the dish if complete
	const bool bWasCorrect = CustomContext->bWasCorrectIngredient;
	const bool bWasNoHit = CustomContext->bWasNoHit;
	bool bIsDishComplete = false;
	const FGameplayTag VeggieTag = CustomContext->VeggieIngredientTag;

	ON_SCOPE_EXIT
	{
		// Broadcast the veggie evaluated message
		FVeggieEvaluatedPayload Payload;
		Payload.bWasCorrectIngredient = bWasCorrect;
		Payload.Dish = this;
		Payload.bWasNoHit = bWasNoHit;
		Payload.bIsDishComplete = bIsDishComplete;
		Payload.IngredientTag = VeggieTag;
		Payload.CurrentIngredients = CurrentIngredients;
		Payload.AccumulatedFlavours = GetCurrentAccumulatedFlavours();
		Payload.CurrentScore = GetCurrentAccumulatedScore();
		UVeggleGameUtils::GetVeggleEventSubsystem(this)->OnVeggieEvaluated.Broadcast(Payload);
	};


	// Part 1: Update the Current Ingredient Map 
	if (!bWasCorrect || bWasNoHit)
	{
		return;
	}
	if (VeggieTag.IsValid())
	{
		CurrentIngredients.FindOrAdd(VeggieTag)++;
	}

	// TODO Handle Incorrect Veggies
	// Part 2: Spawn Veggie Proxy above
	SpawnVeggieProxy(VeggieTag, bWasCorrect);
	
	// Part 3: Check for Completion 
	bool bIsComplete = true;

	// A. Check required ingredients count
	for (const TPair<FGameplayTag, int32>& RequiredPair : Recipe->RequiredIngredients)
	{
	    const int32 CurrentCount = CurrentIngredients.FindRef(RequiredPair.Key);
	    if (CurrentCount < RequiredPair.Value)
	    {
	        bIsComplete = false;
	        break;
	    }
	}

	// Part 4: Finish the Dish
	if (bIsComplete)
	{
		OnDishCompleted.ExecuteIfBound(this, EDishCompletionState::Success);
		bIsDishComplete = true;
	}
}

TMap<FGameplayTag, float> AVeggleDish::GetCurrentAccumulatedFlavours() const
{
	TMap<FGameplayTag, float> FlavourMap;
	FlavourMap.Add(Veggle::Flavour::Butter, AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetAccumulatedButterAttribute()));
	FlavourMap.Add(Veggle::Flavour::Salt, AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetAccumulatedSaltAttribute()));
	FlavourMap.Add(Veggle::Flavour::Pepper, AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetAccumulatedPepperAttribute()));
	FlavourMap.Add(Veggle::Flavour::Heat, AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetAccumulatedHeatAttribute()));
	FlavourMap.Add(Veggle::Flavour::WildcardFlavour, AbilitySystemComponent->GetNumericAttribute(UVeggleDishAttributeSet::GetAccumulatedWildcardFlavourAttribute()));
	return FlavourMap;
}
