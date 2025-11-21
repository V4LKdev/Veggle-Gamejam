// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleAccumulateVeggieData.h"

#include "Veggle/VeggleLogChannels.h"
#include "Veggle/AbilitySystem/VeggleDishAttributeSet.h"
#include "Veggle/AbilitySystem/VeggleGameplayEffectContext.h"
#include "Veggle/AbilitySystem/VeggleVeggieAttributeSet.h"
#include "Veggle/Dish/VeggleDish.h"
#include "Veggle/Dish/VeggleRecipeData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleAccumulateVeggieData)

struct FAccumulateVeggieStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(HitCount);
	DECLARE_ATTRIBUTE_CAPTUREDEF(SpecialMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FillerMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BoardClearedBonusMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Butter);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Salt);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Pepper);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Heat);
	DECLARE_ATTRIBUTE_CAPTUREDEF(WildcardFlavour);

	FAccumulateVeggieStatics()
	{
		// Source captures
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, HitCount, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, SpecialMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, FillerMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, BoardClearedBonusMultiplier, Source, true);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, Butter, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, Salt, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, Pepper, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, Heat, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UVeggleVeggieAttributeSet, WildcardFlavour, Source, true);
	}
};

static FAccumulateVeggieStatics& AccumulateStatics()
{
	static FAccumulateVeggieStatics Statics;
	return Statics;
}

UVeggleAccumulateVeggieData::UVeggleAccumulateVeggieData()
{
	RelevantAttributesToCapture.Add(AccumulateStatics().HitCountDef);
	RelevantAttributesToCapture.Add(AccumulateStatics().SpecialMultiplierDef);
	RelevantAttributesToCapture.Add(AccumulateStatics().FillerMultiplierDef);
	RelevantAttributesToCapture.Add(AccumulateStatics().BoardClearedBonusMultiplierDef);
	
	RelevantAttributesToCapture.Add(AccumulateStatics().ButterDef);
	RelevantAttributesToCapture.Add(AccumulateStatics().SaltDef);
	RelevantAttributesToCapture.Add(AccumulateStatics().PepperDef);
	RelevantAttributesToCapture.Add(AccumulateStatics().HeatDef);
	RelevantAttributesToCapture.Add(AccumulateStatics().WildcardFlavourDef);

	// --- Default Attribute Mapping ---
	AttributeMapping.Add(UVeggleVeggieAttributeSet::GetButterAttribute(), UVeggleDishAttributeSet::GetAccumulatedButterAttribute());
	AttributeMapping.Add(UVeggleVeggieAttributeSet::GetSaltAttribute(), UVeggleDishAttributeSet::GetAccumulatedSaltAttribute());
	AttributeMapping.Add(UVeggleVeggieAttributeSet::GetPepperAttribute(), UVeggleDishAttributeSet::GetAccumulatedPepperAttribute());
	AttributeMapping.Add(UVeggleVeggieAttributeSet::GetHeatAttribute(), UVeggleDishAttributeSet::GetAccumulatedHeatAttribute());
	AttributeMapping.Add(UVeggleVeggieAttributeSet::GetWildcardFlavourAttribute(), UVeggleDishAttributeSet::GetAccumulatedWildcardFlavourAttribute());
	
}


void UVeggleAccumulateVeggieData::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	FVeggleGameplayEffectContext* CustomContext = static_cast<FVeggleGameplayEffectContext*>(ExecutionParams.GetOwningSpec().GetContext().Get());
    if (!SourceASC || !TargetASC || !CustomContext)
	{
		return;
	}
	
    // Get the target Dish Actor to access its recipe
    const AVeggleDish* TargetDish = Cast<AVeggleDish>(TargetASC->GetAvatarActor());
    if (!TargetDish || !TargetDish->GetRecipe())
    {
        return;
    }

	// Check if the source HitCount is 0 and apply penalty
	const float HitCount = GetHitCount(ExecutionParams, OutExecutionOutput, TargetASC, TargetDish, CustomContext);
	if (HitCount <= 0)
	{
		// get the Accumulated scoreAttribute value from the target
		const float TargetScore = TargetDish->GetCurrentAccumulatedScore();

		// Apply penalty and mark as incorrect
		const float ScorePenalty = TargetScore * (NoHitPenaltyPercentage / 100.f);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVeggleDishAttributeSet::GetIncorrectIngredientsAttribute(), EGameplayModOp::Additive, 1.0f));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVeggleDishAttributeSet::GetAccumulatedScoreAttribute(), EGameplayModOp::Additive, -ScorePenalty));
		CustomContext->bWasCorrectIngredient = false;
		CustomContext->bWasNoHit = true;
		UE_LOG(LogVeggleCalculation, Warning, TEXT("[Penalty] No hits for ingredient. Score penalty applied: %f (Current Score: %f, Penalty %%: %f)"), -ScorePenalty, TargetScore, NoHitPenaltyPercentage);
		return;
	}


	// Get the ingredient tag from the spec
    const UVeggleRecipeData* Recipe = TargetDish->GetRecipe();
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTag IngredientTag = GetIngredientTag(Spec);
	if (!IngredientTag.IsValid())
    {
		UE_LOG(LogVeggleCalculation, Warning, TEXT("No valid ingredient tag found in the spec!"));
        return;
    }
	CustomContext->VeggieIngredientTag = IngredientTag;

    // Check if the ingredient is actually needed in the recipe
	const int32* RequiredAmountPtr = Recipe->RequiredIngredients.Find(IngredientTag);
	const int32 CurrentCorrectCount = TargetDish->GetCurrentIngredientCount(IngredientTag);

	// If it's needed, Calculate the FULL final score and apply it to the dish score, then Aggregate the flavours
    if (RequiredAmountPtr && CurrentCorrectCount < *RequiredAmountPtr)
    {
		 HandleCorrectIngredient(ExecutionParams, OutExecutionOutput, TargetASC, CustomContext, Spec, HitCount);
    }
	// If it's not needed, calculate the score without the specialMult and apply it to the dish score, do NOT aggregate the flavours
    else
    {
        HandleIncorrectIngredient(ExecutionParams, OutExecutionOutput, TargetASC, CustomContext, Spec, HitCount);
    }
}


float UVeggleAccumulateVeggieData::GetHitCount(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput, UAbilitySystemComponent* TargetASC, const AVeggleDish* TargetDish, FVeggleGameplayEffectContext* CustomContext)
{
	FAggregatorEvaluateParameters ScoreEvalParams;
	ScoreEvalParams.SourceTags = ExecutionParams.GetOwningSpec().CapturedSourceTags.GetAggregatedTags();
	ScoreEvalParams.TargetTags = &TargetASC->GetOwnedGameplayTags();
	float HitCount = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AccumulateStatics().HitCountDef, ScoreEvalParams, HitCount);
	return HitCount;
}

FGameplayTag UVeggleAccumulateVeggieData::GetIngredientTag(const FGameplayEffectSpec& Spec)
{
	FGameplayTag IngredientTag;
	
	for (const FGameplayTag& Tag : *Spec.CapturedSourceTags.GetAggregatedTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Veggle.Vegetable"))))
		{
			IngredientTag = Tag;
			break;
		}
	}
	return IngredientTag;
}

void UVeggleAccumulateVeggieData::HandleCorrectIngredient(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput, UAbilitySystemComponent* TargetASC, FVeggleGameplayEffectContext* CustomContext, const FGameplayEffectSpec& Spec, float HitCount) const
{
	// --- CORRECT INGREDIENT ---
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVeggleDishAttributeSet::GetCorrectIngredientsAttribute(), EGameplayModOp::Additive, 1.0f));
	CustomContext->bWasCorrectIngredient = true;

	// --- CALCULATE BALL FINAL SCORE ---
	const float FinalBallScore = CalcFinalBallScore(ExecutionParams, HitCount, true);
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVeggleDishAttributeSet::GetAccumulatedScoreAttribute(), EGameplayModOp::Additive, FinalBallScore));
	
	// ---  DYNAMICALLY ACCUMULATE STATS ---
	FAggregatorEvaluateParameters EvalParameters;
	EvalParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParameters.TargetTags = &TargetASC->GetOwnedGameplayTags();

	for (const TPair<FGameplayAttribute, FGameplayAttribute>& Pair : AttributeMapping)
	{
		const FGameplayAttribute& SourceAttribute = Pair.Key;
		const FGameplayAttribute& TargetAttribute = Pair.Value;
		float Magnitude = 0.f;

		// --- Find the Correct Capture Definition ---
		const FGameplayEffectAttributeCaptureDefinition* FoundCaptureDef = nullptr;
		for (const FGameplayEffectAttributeCaptureDefinition& CaptureDef : RelevantAttributesToCapture)
		{
			if (CaptureDef.AttributeToCapture == SourceAttribute)
			{
				FoundCaptureDef = &CaptureDef;
				break;
			}
		}

		// If we found the matching definition, attempt to get its captured value.
		if (FoundCaptureDef)
		{
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(*FoundCaptureDef, EvalParameters, Magnitude);
		}
        
		// If the captured value is greater than zero, add it to the dish.
		if (Magnitude > 0.f)
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(TargetAttribute, EGameplayModOp::Additive, Magnitude));
		}
	}

	UE_LOG(LogVeggleCalculation, Log, TEXT("[Correct] Ingredient: %s | HitCount: %f | FinalBallScore: %f"), *CustomContext->VeggieIngredientTag.ToString(), HitCount, FinalBallScore);
}

void UVeggleAccumulateVeggieData::HandleIncorrectIngredient(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput, UAbilitySystemComponent* TargetASC, FVeggleGameplayEffectContext* CustomContext, const FGameplayEffectSpec& Spec, float HitCount) const
{
	// --- INCORRECT OR EXTRA INGREDIENT ---
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVeggleDishAttributeSet::GetIncorrectIngredientsAttribute(), EGameplayModOp::Additive, 1.0f));
	CustomContext->bWasCorrectIngredient = false;

	const float FinalBallScore = CalcFinalBallScore(ExecutionParams, HitCount, false);

	// add the score with percentage penalty
	const float PenalizedScore = FinalBallScore * WrongIngredientPenaltyMultiplier;
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UVeggleDishAttributeSet::GetAccumulatedScoreAttribute(), EGameplayModOp::Additive, PenalizedScore));

	UE_LOG(LogVeggleCalculation, Log, TEXT("[Incorrect] Ingredient: %s | HitCount: %f | FinalBallScore: %f | PenalizedScore: %f"), *CustomContext->VeggieIngredientTag.ToString(), HitCount, FinalBallScore, PenalizedScore);
}

float UVeggleAccumulateVeggieData::CalcFinalBallScore(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const float HitCount, const bool bWasRightIngredient)
{
	float SpecialMult = 0.f;
	float FillerMult = 0.f;
	float BoardClearBonus = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AccumulateStatics().SpecialMultiplierDef, FAggregatorEvaluateParameters(), SpecialMult);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AccumulateStatics().FillerMultiplierDef, FAggregatorEvaluateParameters(), FillerMult);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AccumulateStatics().BoardClearedBonusMultiplierDef, FAggregatorEvaluateParameters(), BoardClearBonus);
	
	if (!bWasRightIngredient)
	{
		SpecialMult = 1.f;
	}

	return HitCount * FillerMult * SpecialMult * BoardClearBonus;
}
