// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VeggleWalker.generated.h"

class AVeggleWalkerSpawner;
class UVeggleWalkingVeggieData;
class UCapsuleComponent;

UCLASS()
class VEGGLE_API AVeggleWalker : public AActor
{
	GENERATED_BODY()

public:
	AVeggleWalker();
	virtual void Destroyed() override;
	void InitVeggieWalker(UVeggleWalkingVeggieData* NewVeggieType, AVeggleWalkerSpawner* Spawner);
	
	UFUNCTION(BlueprintCallable, Category = "Veggie Walker")
	UVeggleWalkingVeggieData* GetVeggieType() const { return VeggieType; }

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnVeggieHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(BlueprintImplementableEvent, Category = "Veggie Walker")
	void BP_OnVeggieClicked(AActor* TouchedActor, FKey ButtonPressed);
	UFUNCTION(BlueprintImplementableEvent, Category = "Veggie Walker")
	void BP_OnVeggieHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(BlueprintPure, Category = "Veggie Walker")
	bool GetRandomHopLeft() const;
	UFUNCTION(BlueprintPure, Category = "Veggie Walker")
	float GetRandomTimeToNextHop() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Veggie Walker")
	void BP_OnGameEnded();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Veggie Walker")
	TObjectPtr<UCapsuleComponent> CapsuleCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Veggie Walker")
	TObjectPtr<UStaticMeshComponent> VeggieMesh;

private:
	UPROPERTY()
	TObjectPtr<UVeggleWalkingVeggieData> VeggieType;
	TWeakObjectPtr<AVeggleWalkerSpawner> SpawnerRef;

	UFUNCTION()
	void OnGameEnded();

	UPROPERTY(BlueprintReadOnly, meta =(AllowPrivateAccess = "true"))
	bool bGameEnded = false;
};
