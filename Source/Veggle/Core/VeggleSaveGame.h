// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VeggleSaveGame.generated.h"

class UVeggleLevelData;

UENUM(BlueprintType)
enum class ELevelStatus : uint8
{
	LS_Uninitialized = 0 UMETA(Hidden),
	LS_Locked = 1     UMETA(DisplayName = "Locked"),
	LS_Unlocked = 2    UMETA(DisplayName = "Unlocked"),
	LS_Completed0 = 3   UMETA(DisplayName = "CompletedNoStar"),
	LS_Completed1 = 4   UMETA(DisplayName = "CompletedOneStar"),
	LS_Completed2 = 5   UMETA(DisplayName = "CompletedTwoStars"),
	LS_Completed3 = 6   UMETA(DisplayName = "CompletedThreeStars")
};

USTRUCT(BlueprintType)
struct FLevelInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelInfo")
	ELevelStatus Status = ELevelStatus::LS_Locked;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelInfo")
	TObjectPtr<UVeggleLevelData> LevelData = nullptr;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Config = Game, DefaultConfig)
class VEGGLE_API UVeggleSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame")
	float MusicVolume = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame")
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame")
	TArray<FLevelInfo> LevelInfos;
};
