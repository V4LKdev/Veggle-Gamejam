// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VeggleWalkerSpawner.generated.h"

class AVeggleGameState;
class UVeggleWalkingVeggieData;

UCLASS()
class VEGGLE_API AVeggleWalkerSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AVeggleWalkerSpawner();
	void InitSpawner(const TMap<UVeggleWalkingVeggieData*, float>& InVeggieWalkerTypes, const int32 InMaxVeggies, const float InStartSpawnInterval, const float InEndSpawnInterval, const float InRampUpTime = 100.f);
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnVeggieDestroyed(AActor* DestroyedActor);

	void StartSpawner();
	void StopSpawner();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "Veggie Walker Spawner")
	void TrySpawnVeggieWalker();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Veggie Walker Spawner", meta=(MakeEditWidget = true))
	FVector SpawnLocationA;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Veggie Walker Spawner", meta=(MakeEditWidget = true))
	FVector SpawnLocationB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Veggie Walker Spawner")
	TSubclassOf<AActor> VeggieWalkerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Veggie Walker Spawner")
	float SpawnVarianceX = 40.0f; // Slight variance in X so they don't overlap that much

private:
	void SpawnVeggieWalker_Internal(UVeggleWalkingVeggieData* VeggieType);
	FVector GetRandomSpawnLocation() const;
	UVeggleWalkingVeggieData* GetRandomVeggieWalkerTypeWeighted() const;

	void RampSpawnInterval(float DeltaTime);
	
	UPROPERTY()
	TArray<AActor*> SpawnedVeggies;
	
	UPROPERTY()
	TMap<UVeggleWalkingVeggieData*, float> VeggieWalkerTypes;
	int32 MaxVeggies;
	float StartSpawnInterval;
	float EndSpawnInterval;
	float CurrentSpawnInterval;
	float RampUpTime;
	float ElapsedRampTime = 0.0f;

	float SpawnTimer = 0.0f;
};
