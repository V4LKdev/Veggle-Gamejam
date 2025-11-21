// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleGameUtils.h"

#include "Core/VeggleGameInstance.h"
#include "Core/VeggleGameState.h"
#include "Core/VeggleSaveGame.h"
#include "Core/Data/VeggleLevelData.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleGameUtils)

AVeggleGameState* UVeggleGameUtils::GetVeggleGameState(const UObject* WorldContextObject)
{
	return Cast<AVeggleGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}

UVeggleEventSubsystem* UVeggleGameUtils::GetVeggleEventSubsystem(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetSubsystem<UVeggleEventSubsystem>();
}

float UVeggleGameUtils::GetPercentageUntilOptimalDishCompletion(const float TimeRemaining, const float TotalTime, const float OptimalTime)
{
	const float Elapsed = TotalTime - TimeRemaining;
	return FMath::Clamp(Elapsed / OptimalTime, 0.0f, 1.0f);
}

float UVeggleGameUtils::GetPercentageUntilDishCompletionFromOptimal(const float TimeRemaining, const float TotalTime, const float OptimalTime)
{
	const float Elapsed = TotalTime - TimeRemaining;
	return FMath::Clamp((Elapsed - OptimalTime) / (TotalTime - OptimalTime), 0.0f, 1.0f);
}

UE_DISABLE_OPTIMIZATION
void UVeggleGameUtils::UpdateLevelInfo(const UObject* WorldContextObject, const float PointsScored, const FText& LevelName)
{
	// Get game instance and get save game from that
	UVeggleGameInstance* VeggleGameInstance = Cast<UVeggleGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (!VeggleGameInstance)
	{
		return;
	}

	FString SaveSlotName = VeggleGameInstance->GetSaveGameSlotName();
	
	UVeggleSaveGame* SaveGame = Cast<UVeggleSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!SaveGame)
	{
		return;
	}

	// go over the save game LevelInfos array to find the one that matches the current level
	for (int32 i = 0; i < SaveGame->LevelInfos.Num(); ++i)
	{
		FLevelInfo& LevelInfo = SaveGame->LevelInfos[i];
		if (LevelInfo.LevelData->LevelName.EqualTo(LevelName))
		{
			ELevelStatus LevelStatus;
			if (PointsScored >= LevelInfo.LevelData->PointsForThreeStars)
			{
				LevelStatus = ELevelStatus::LS_Completed3;
			}
			else if (PointsScored >= LevelInfo.LevelData->PointsForTwoStars)
			{
				LevelStatus = ELevelStatus::LS_Completed2;
			}
			else if (PointsScored >= LevelInfo.LevelData->PointsForOneStar)
			{
				LevelStatus = ELevelStatus::LS_Completed1;
			}
			else
			{
				LevelStatus = ELevelStatus::LS_Unlocked;
			}

			LevelInfo.Status = FMath::Max(LevelInfo.Status, LevelStatus);

			// Unlock the next level if it is locked
			if (i + 1 < SaveGame->LevelInfos.Num())
			{
				FLevelInfo& NextLevelInfo = SaveGame->LevelInfos[i + 1];
				if (NextLevelInfo.Status == ELevelStatus::LS_Locked)
				{
					NextLevelInfo.Status = ELevelStatus::LS_Unlocked;
				}
			}
			break;
		}
	}

	// Save the updated save game
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);
}
