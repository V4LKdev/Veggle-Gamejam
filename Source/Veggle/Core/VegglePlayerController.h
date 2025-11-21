// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VegglePlayerController.generated.h"

class AVeggleLauncherPawn;
struct FInputActionValue;
/**
 * Game flow responsibilities:
 * Click detection for walking veggies and firing the peg
 * Telling the Ball launcher to load the ball with correct data
 */
UCLASS()
class VEGGLE_API AVegglePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVegglePlayerController();
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Veggle|PlayerController")
	TObjectPtr<class UInputMappingContext> IMC;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Veggle|PlayerController")
	TObjectPtr<class UInputAction> IA_Select;

	UFUNCTION()
	void OnSelect(const FInputActionValue& Value);

	UFUNCTION()
	void OnGameEnded();

private:
	void HighlightObjectsUnderCursor() const;

	void HighlightActor(const AActor* Actor, bool bHighlight) const;

	UPROPERTY()
	TObjectPtr<AActor> HighlightedActor;
	UPROPERTY()
	TObjectPtr<AActor> PreviousHighlightedActor;
	UPROPERTY()
	TObjectPtr<AVeggleLauncherPawn> LauncherPawn;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Veggle|PlayerController")
	int32 OutlineDepthStencilValue = 15;

	bool bCanHighlight = true;
};
