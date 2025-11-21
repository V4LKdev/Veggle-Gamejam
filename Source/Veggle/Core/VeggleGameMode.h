// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "VeggleGameMode.generated.h"

enum class EDishCompletionState : uint8;
class AVeggleGameState;
class AVeggleDish;
class UVeggleLevelData;
/**
 * Game flow responsibilities:
 * Decide which dish to serve next based on level data
 * Spawning the dish actors
 * Telling the game state the current dish
 * Evaluating a send dish
 * Deciding level completion
 */
UCLASS()
class VEGGLE_API AVeggleGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AVeggleGameMode();
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	
	UVeggleLevelData* GetLevelData() const { return LevelData; }

protected:
	virtual void StartPlay() override;
	
	/** GameMode Interface */
	virtual void HandleMatchIsWaitingToStart() override;	// Pregame
	virtual void HandleMatchHasStarted() override;			// Midgame
	virtual void HandleMatchHasEnded() override;			// Postgame
	virtual void OnMatchStateSet() override;
	
	
	UFUNCTION(BlueprintCallable, Category = "Veggle|GameMode")
	void SpawnNextDish();

	UPROPERTY(EditAnywhere, Category = "LevelData")
	TSubclassOf<AVeggleDish> DishClass;

private:
	UFUNCTION()
	void OnPregameCountdownStarted();
	UFUNCTION()
	void ProcessCompleteDish(AVeggleDish* Dish, EDishCompletionState CompletionState);
	UFUNCTION()
	void OnDishServed();
	
	UPROPERTY()
	TObjectPtr<UVeggleLevelData> LevelData;

	AVeggleGameState* GetVeggleGameState();
	UPROPERTY()
	TObjectPtr<AVeggleGameState> VeggleGameState;

	int32 CurrentDishIndex = 0;
	UPROPERTY()
	TObjectPtr<AActor> DishSpawnPoint;
};
