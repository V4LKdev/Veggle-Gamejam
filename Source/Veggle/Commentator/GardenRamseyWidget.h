// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VeggleCommentData.h"
#include "Blueprint/UserWidget.h"
#include "Veggle/Events/VeggleEventPayloads.h"
#include "GardenRamseyWidget.generated.h"

class UImage;
class AVeggleDish;


UENUM(BlueprintType)
enum class EVeggleVoiceLineImportance : uint8
{
	Low,
	Normal,
	High
};

struct FQueuedVoiceLine
{
	FVeggleCommentData CommentData;
	EVeggleVoiceLineImportance Importance;

	FQueuedVoiceLine(const FVeggleCommentData& InData, EVeggleVoiceLineImportance InImportance)
		: CommentData(InData), Importance(InImportance) {}
};


/**
 * 
 */
UCLASS()
class VEGGLE_API UGardenRamseyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GardenPortrait;
	
private:
	UFUNCTION()
	void OnNewDish(AVeggleDish* Dish);
	UFUNCTION()
	void OnPregameCountdownStarted();
	UFUNCTION()
	void OnPostgameStarted();
	UFUNCTION()
	void OnVeggieHitPeg(const FVeggieHitPegPayload& Payload);
	UFUNCTION()
	void OnVeggieEvaluated(const FVeggieEvaluatedPayload& Payload);
	UFUNCTION()
	void OnDishCompleted(const FDishCompletedPayload& Payload);
	
	static FVeggleCommentData GetRandomVoiceLine(const UDataTable* DataTable, float Probability = 1.0f);
	static FVeggleCommentData GetRandomVoiceLineWithContext(const UDataTable* DataTable, const FGameplayTagContainer& ContextTags, float Probability = 1.0f);
	
	void QueueVoiceLine(const FVeggleCommentData& CommentData, const EVeggleVoiceLineImportance Importance);
	UFUNCTION()
	void PlayNextVoiceLine();
	void PlayVoiceLine(const FVeggleCommentData& CommentData);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> NewDishLines;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PegHitLines;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> VeggieEvaluatedLines;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> DishCompletedLines;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PregameCountdownLines;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PostgameLines;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PegBoardClearedLines;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> IdleLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float PegHitProbability = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float NoHitProbability = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float CorrectIngredientProbability = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float IncorrectIngredientProbability = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float PerfectDishProbability = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float TimeOutDishProbability = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float NormalDishProbability = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float IdleProbability = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Garden Ramsey", meta = (AllowPrivateAccess = "true"))
	float TimeUntilNextIdleVoiceLine = 10.0f;

	float TimeSinceLastVoiceLine = 0.0f;

	TArray<FQueuedVoiceLine> VoiceLineQueue;
	bool bIsPlayingVoiceLine = false;
	bool bIsInGame = false;
};
