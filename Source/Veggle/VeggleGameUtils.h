// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/VeggleEventSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VeggleGameUtils.generated.h"

class AVeggleGameState;
/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleGameUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Veggle|Utils", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Veggle Game State"))
	static AVeggleGameState* GetVeggleGameState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Veggle|Utils", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Veggle Event Subsystem"))
	static UVeggleEventSubsystem* GetVeggleEventSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Veggle|Utils")
	static float GetPercentageUntilOptimalDishCompletion(float TimeRemaining, float TotalTime, float OptimalTime);
	UFUNCTION(BlueprintPure, Category = "Veggle|Utils")
	static float GetPercentageUntilDishCompletionFromOptimal(float TimeRemaining, float TotalTime, float OptimalTime);

	UFUNCTION(BlueprintCallable, Category = "Veggle|Utils", meta = (WorldContext = "WorldContextObject", DisplayName = "Update Level Info"))
	static void UpdateLevelInfo(const UObject* WorldContextObject, const float PointsScored, const FText& LevelName);
};
