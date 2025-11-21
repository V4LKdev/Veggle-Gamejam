// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "VeggleBallData.generated.h"

UCLASS()
class VEGGLE_API UVeggleBallData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball Appearance", meta=(Categories="Veggle.Vegetable"))
	FGameplayTag VeggieType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball Appearance")
	TObjectPtr<UTexture2D> BallTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball Behaviour")
	float LaunchStrength = 1500.0f;
};
