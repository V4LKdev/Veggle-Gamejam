// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VeggleEventSubsystem.generated.h"

struct FVeggieEvaluatedPayload;
struct FVeggieHitPegPayload;
struct FVeggieLoadedPayload;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVeggieLoadedSignature, const FVeggieLoadedPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVeggieHitPegSignature, const FVeggieHitPegPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVeggieEvaluatedSignature, const FVeggieEvaluatedPayload&, Payload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDishCompletedSignature, const FDishCompletedPayload&, Payload);

UCLASS()
class VEGGLE_API UVeggleEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// EVENT CHANNELS
	UPROPERTY(BlueprintAssignable, Category = "Veggle|Events")
	FOnVeggieLoadedSignature OnVeggieLoaded;
	UPROPERTY(BlueprintAssignable, Category = "Veggle|Events")
	FOnVeggieHitPegSignature OnVeggieHitPeg;
	UPROPERTY(BlueprintAssignable, Category = "Veggle|Events")
	FOnVeggieEvaluatedSignature OnVeggieEvaluated;
	UPROPERTY(BlueprintAssignable, Category = "Veggle|Events")
	FOnDishCompletedSignature OnDishCompleted;
};
