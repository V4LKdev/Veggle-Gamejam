// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VeggleBallCollector.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class VEGGLE_API AVeggleBallCollector : public AActor
{
	GENERATED_BODY()

public:
	AVeggleBallCollector();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> CollectorCollisionBox;

private:
	UFUNCTION()
	void OnBallOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
