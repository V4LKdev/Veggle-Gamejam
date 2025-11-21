// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "VeggleGameState.generated.h"

class AVeggleWalkerSpawner;
class AVegglePegboard;
class UVeggleLevelData;
class UVeggleWalkingVeggieData;
class AVeggleDish;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewDish, AVeggleDish*, Dish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateDishTimer, float, TimeRemaining, float, TotalTime, float, OptimalTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDishTimerExpired, AVeggleDish*, Dish, EDishCompletionState, CompletionState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPregameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostgameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPregameCountdownStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPregameCountdownTick, int32, TimeRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPregameCountdownFinished);

/**
 * Game flow responsibilities:
 * Reference to the current dish
 * Holding the current score
 * Holding and managing dish timer
 * Tracking how many dishes have been served
 */
UCLASS()
class VEGGLE_API AVeggleGameState : public AGameState
{
	GENERATED_BODY()

	friend class AVeggleGameMode;
	
public:
	AVeggleGameState();
	
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Veggle|GameState")
	void StartPregameCountdown(float Duration);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnNewDish OnNewDish;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FUpdateDishTimer OnUpdateDishTimer;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDishTimerExpired OnDishTimerExpired;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPregameStarted OnPregameStarted;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMatchStarted OnMatchStarted;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPostgameStarted OnPostgameStarted;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPregameCountdownStarted OnPregameCountdownStarted;
	UPROPERTY(BlueprintAssignable , Category = "Events")
	FOnPregameCountdownTick OnPregameCountdownTick;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPregameCountdownFinished OnPregameCountdownFinished;
	
	AVegglePegboard* GetPegboard() const { return Pegboard.Get(); }

	UFUNCTION(BlueprintPure, Category = "Score")
	float GetCurrentDishTimeRemaining() const { return CurrentDishTimeRemaining; }
	UFUNCTION(BlueprintPure, Category = "Score")
	double GetTotalScore() const { return TotalScore; }
	AVeggleDish* GetActiveDish() const { return ActiveDish.Get(); }
protected:
	virtual void OnRep_MatchState() override;
	
private:
	void SetActiveDish(AVeggleDish* Dish);
	void SetPegboard(AVegglePegboard* InPegboard);
	void SetWalkerSpawner(AVeggleWalkerSpawner* InWalkerSpawner);
	void AddToTotalScore(const float Score) { TotalScore += Score; }
	
	
	TWeakObjectPtr<AVeggleDish> ActiveDish;
	TWeakObjectPtr<UVeggleLevelData> LevelData;
	TWeakObjectPtr<AVegglePegboard> Pegboard;
	TWeakObjectPtr<AVeggleWalkerSpawner> WalkerSpawner;
	
	float CurrentDishTimeRemaining;
	double TotalScore = 0.0f;

	int32 PregameCountdownTimeRemaining;
	FTimerHandle PregameCountdownTimerHandle;
	void UpdatePregameCountdown();
	
};
