// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VeggleWalkingVeggieData.generated.h"

class UVeggleBallData;
/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleWalkingVeggieData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMesh> VeggieMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UVeggleBallData> BallData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour")
	float HopUpStrength = 700.0f; // Strength of the hop impulse applied to the veggie
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour")
	float HopSideStrength = 500.0f; // Side impulse applied to the veggie when
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour")
	float HopCooldownMin = 0.5f; // Minimum time between hops
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour")
	float HopCooldownMax = 1.5f; // Maximum time between hops
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour")
	TObjectPtr<UPhysicalMaterial> PhysicsMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float LandingZScale = 0.85f; // Scale applied to the Z axis when the veggie lands
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float HopZScale = 1.5f; // Scale applied to the Z axis when the veggie hops
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float LandingSpeedMultiplier = 1.0f; // Speed multiplier for the landing animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float HopSpeedMultiplier = 1.0f; // Speed multiplier for the hop animation
};
