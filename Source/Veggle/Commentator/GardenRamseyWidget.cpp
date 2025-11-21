// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "GardenRamseyWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Veggle/VeggleGameplayTags.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/VeggleLogChannels.h"
#include "Veggle/Core/VeggleGameState.h"
#include "Veggle/Pegboard/Peg/VegglePegData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GardenRamseyWidget)

void UGardenRamseyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind to all the callbacks:
	AVeggleGameState* GameState = UVeggleGameUtils::GetVeggleGameState(this);
	if (!IsValid(GameState))
	{
		return;
	}

	GameState->OnNewDish.AddDynamic(this, &ThisClass::OnNewDish);
	GameState->OnPregameCountdownStarted.AddDynamic(this, &ThisClass::OnPregameCountdownStarted);
	GameState->OnPostgameStarted.AddDynamic(this, &ThisClass::OnPostgameStarted);

	UVeggleEventSubsystem* VeggleEvent = UVeggleGameUtils::GetVeggleEventSubsystem(this);
	if (!IsValid(VeggleEvent))
	{
		return;
	}

	VeggleEvent->OnVeggieHitPeg.AddDynamic(this, &ThisClass::OnVeggieHitPeg);
	VeggleEvent->OnVeggieEvaluated.AddDynamic(this, &ThisClass::OnVeggieEvaluated);
	VeggleEvent->OnDishCompleted.AddDynamic(this, &ThisClass::OnDishCompleted);
	
}

void UGardenRamseyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsInGame)
	{
		return;
	}
	
	TimeSinceLastVoiceLine += InDeltaTime;
	
	if (TimeSinceLastVoiceLine > TimeUntilNextIdleVoiceLine)
	{
		QueueVoiceLine(GetRandomVoiceLine(IdleLines, IdleProbability), EVeggleVoiceLineImportance::Low);
	}
}

void UGardenRamseyWidget::OnNewDish(AVeggleDish* Dish)
{
	QueueVoiceLine(GetRandomVoiceLine(NewDishLines), EVeggleVoiceLineImportance::High);
}

void UGardenRamseyWidget::OnPregameCountdownStarted()
{
	QueueVoiceLine(GetRandomVoiceLine(PregameCountdownLines), EVeggleVoiceLineImportance::High);
	bIsInGame = true;
}

void UGardenRamseyWidget::OnPostgameStarted()
{
	QueueVoiceLine(GetRandomVoiceLine(PostgameLines), EVeggleVoiceLineImportance::High);
	bIsInGame = false;
}

void UGardenRamseyWidget::OnVeggieHitPeg(const FVeggieHitPegPayload& Payload)
{
	if (Payload.bBoardCleared)
	{
		QueueVoiceLine(GetRandomVoiceLine(PegBoardClearedLines), EVeggleVoiceLineImportance::Normal);
		return;
	}

	FGameplayTagContainer ContextTags;
	
	// Calculate if it was a special peg that was hit
	const FGameplayTag PegTypeTag = Payload.PegType->PegTypeTag;
	if (PegTypeTag.IsValid())
	{
		// check how many of this tag the ball has
		const int32 TagCount = Payload.BallASC->GetTagCount(PegTypeTag);
		if (TagCount > 1)
		{
			ContextTags.AddTag(Veggle::EventContext::Negative);
		}
		else
		{
			ContextTags.AddTag(Veggle::EventContext::Positive);
		}

		QueueVoiceLine(GetRandomVoiceLineWithContext(PegHitLines, ContextTags, PegHitProbability), EVeggleVoiceLineImportance::Low);
	}
}

void UGardenRamseyWidget::OnVeggieEvaluated(const FVeggieEvaluatedPayload& Payload)
{
	FGameplayTagContainer ContextTags;
	float Probability;
	
	if (Payload.bWasNoHit)
	{
		ContextTags.AddTag(Veggle::EventContext::VeryBad);
		Probability = NoHitProbability;
	}
	else if (Payload.bWasCorrectIngredient)
	{
		ContextTags.AddTag(Veggle::EventContext::Positive);
		Probability = CorrectIngredientProbability;
	}
	else
	{
		ContextTags.AddTag(Veggle::EventContext::Negative);
		Probability = IncorrectIngredientProbability;
	}
	
	QueueVoiceLine(GetRandomVoiceLineWithContext(VeggieEvaluatedLines, ContextTags, Probability), EVeggleVoiceLineImportance::Low);
}

void UGardenRamseyWidget::OnDishCompleted(const FDishCompletedPayload& Payload)
{
	FGameplayTagContainer ContextTags;
	float Probability;
	
	if (Payload.bIsPerfect)
	{
		ContextTags.AddTag(Veggle::EventContext::Positive);
		Probability = PerfectDishProbability;
	}
	else if (Payload.bWasTimeOut)
	{
		ContextTags.AddTag(Veggle::EventContext::Negative);
		Probability = TimeOutDishProbability;
	}
	else
	{
		ContextTags.AddTag(Veggle::EventContext::Neutral);
		Probability = NormalDishProbability;
	}
	
	QueueVoiceLine(GetRandomVoiceLineWithContext(DishCompletedLines, ContextTags, Probability), EVeggleVoiceLineImportance::Normal);
}


FVeggleCommentData UGardenRamseyWidget::GetRandomVoiceLine(const UDataTable* DataTable, const float Probability)
{
	if (!DataTable) return FVeggleCommentData();

	if (FMath::FRand() >= Probability)
		return FVeggleCommentData();

	TArray<FName> RowNames = DataTable->GetRowNames();
	if (RowNames.Num() == 0) return FVeggleCommentData();

	const int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
	const FName SelectedRow = RowNames[RandomIndex];

	FVeggleCommentData* Row = DataTable->FindRow<FVeggleCommentData>(SelectedRow, TEXT("GetRandomRowFromDataTable"));
	return Row ? *Row : FVeggleCommentData();
}

FVeggleCommentData UGardenRamseyWidget::GetRandomVoiceLineWithContext(const UDataTable* DataTable, const FGameplayTagContainer& ContextTags, const float Probability)
{
	if (!DataTable) return FVeggleCommentData();

	if (FMath::FRand() >= Probability)
		return FVeggleCommentData();

	TArray<FName> RowNames = DataTable->GetRowNames();
	TArray<FVeggleCommentData> MatchingRows;

	for (const FName& RowName : RowNames)
	{
		const FVeggleCommentData* Row = DataTable->FindRow<FVeggleCommentData>(RowName, TEXT("GetRandomVoiceLineWithContext"));
		if (Row && Row->ContextTags.HasAll(ContextTags))
		{
			MatchingRows.Add(*Row);
		}
	}

	if (MatchingRows.Num() == 0) return FVeggleCommentData();

	const int32 RandomIndex = FMath::RandRange(0, MatchingRows.Num() - 1);
	return MatchingRows[RandomIndex];
}

void UGardenRamseyWidget::QueueVoiceLine(const FVeggleCommentData& CommentData, const EVeggleVoiceLineImportance Importance)
{
	VoiceLineQueue.Add(FQueuedVoiceLine(CommentData, Importance));

	// If queue exceeds 3, discard middle low-importance lines
	if (VoiceLineQueue.Num() > 3)
	{
		// Keep first and last, remove middle if not High importance
		for (int32 i = 1; i < VoiceLineQueue.Num() - 1;)
		{
			if (VoiceLineQueue[i].Importance == EVeggleVoiceLineImportance::Low)
			{
				VoiceLineQueue.RemoveAt(i);
			}
			else
			{
				++i;
			}
		}
		// If still >3, remove oldest non-high importance
		while (VoiceLineQueue.Num() > 3)
		{
			for (int32 i = 1; i < VoiceLineQueue.Num() - 1; ++i)
			{
				if (VoiceLineQueue[i].Importance != EVeggleVoiceLineImportance::High)
				{
					VoiceLineQueue.RemoveAt(i);
					break;
				}
			}
		}
	}

	if (!bIsPlayingVoiceLine)
	{
		PlayNextVoiceLine();
	}
}

void UGardenRamseyWidget::PlayNextVoiceLine()
{
	if (VoiceLineQueue.Num() == 0)
	{
		bIsPlayingVoiceLine = false;
		return;
	}

	bIsPlayingVoiceLine = true;
	const FQueuedVoiceLine NextLine = VoiceLineQueue[0];
	VoiceLineQueue.RemoveAt(0);

	PlayVoiceLine(NextLine.CommentData);
}

void UGardenRamseyWidget::PlayVoiceLine(const FVeggleCommentData& CommentData)
{
	TimeSinceLastVoiceLine = 0.0f;
	
	if (CommentData.VoiceLine)
	{
		UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(this, CommentData.VoiceLine);

		if (AudioComp)
		{
			AudioComp->OnAudioFinished.AddDynamic(this, &UGardenRamseyWidget::PlayNextVoiceLine);
		}
		else
		{
			PlayNextVoiceLine();
		}
	}
	else
	{
		PlayNextVoiceLine();
	}
	
	if (!CommentData.Subtitle.IsEmpty())
	{
		UE_LOG(LogVeggle, Log, TEXT("Garden Ramsey: %s"), *CommentData.Subtitle.ToString());
	}
	if (CommentData.PortraitImage)
	{
		GardenPortrait->SetBrushFromTexture(CommentData.PortraitImage);
	}
}

