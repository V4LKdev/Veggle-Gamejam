// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VeggleLauncherPawn.generated.h"

class UWidgetComponent;
class UVeggleBallData;
class AVeggleBall;
class AVegglePlayerController;
class UArrowComponent;

UCLASS()
class VEGGLE_API AVeggleLauncherPawn : public APawn
{
	GENERATED_BODY()

public:
	AVeggleLauncherPawn();
	virtual void Tick(float DeltaTime) override;

	void ReloadBall(UVeggleBallData* BallType);
	void LaunchBall();
	bool IsLoaded() const { return bIsLoaded; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UWidgetComponent> AimWidgetComponent;
	UPROPERTY(Transient)
	TObjectPtr<const AVegglePlayerController> CachedPlayerController;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Veggle")
	bool bAllowAimAbove = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Veggle", meta = (ClampMin = "0.0"))
	float MinAimDistance = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Veggle")
	TSubclassOf<AVeggleBall> BallClass;

private:
	void Aim() const;
	
	bool bIsLoaded = false;
	UPROPERTY(Transient)
	TObjectPtr<AVeggleBall> CurrentBall;
	
};
