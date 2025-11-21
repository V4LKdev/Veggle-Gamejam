// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VeggleLevelData.generated.h"

class UVeggleWalkingVeggieData;
class UVeggleRecipeData;

UCLASS()
class VEGGLE_API UVeggleLevelData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	FText LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	TObjectPtr<UDataTable> PegboardPattern;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	TArray<TObjectPtr<UVeggleRecipeData>> DishSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	float PointsForOneStar = 50000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	float PointsForTwoStars = 80000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData")
	float PointsForThreeStars = 150000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData|VeggieWalker")
	int32 MaxVeggiesOnTable = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData|VeggieWalker", meta = (Units = "s"))
	float VeggieWalkerStartSpawnInterval = 2.0f;
	UPROPERTY(EditAnywhere, BLueprintReadOnly, Category = "LevelData|VeggieWalker", meta = (Units = "s"))
	float VeggieWalkerEndSpawnInterval = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData|VeggieWalker", meta =(Units = "s"))
	float RampUpTime = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelData|VeggieWalker")
	TMap<UVeggleWalkingVeggieData*, float> VeggieWalkerTypes;
};

// TODO: define Wildcard flavour here