// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleGameState.h"

#include "Veggle/Dish/VeggleDish.h"
#include "Veggle/Dish/VeggleRecipeData.h"
// These are needed for the weak obj ptrs
#include "Data/VeggleLevelData.h"
#include "GameFramework/GameMode.h"
#include "Veggle/Pegboard/Board/VegglePegboard.h"
#include "Veggle/WalkingVeggies/VeggleWalkerSpawner.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleGameState)

AVeggleGameState::AVeggleGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentDishTimeRemaining = 0.0f;
}

void AVeggleGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		OnPregameStarted.Broadcast();
	}
	else if (GetMatchState() == MatchState::InProgress)
	{
		OnMatchStarted.Broadcast();
	}
	else if (GetMatchState() == MatchState::WaitingPostMatch)
	{
		OnPostgameStarted.Broadcast();
	}
}

void AVeggleGameState::SetActiveDish(AVeggleDish* Dish)
{
	ActiveDish = Dish;

	if (IsValid(Dish) && Dish->GetRecipe())
	{
		CurrentDishTimeRemaining = Dish->GetRecipe()->MaxPreparationTime;
		OnNewDish.Broadcast(Dish);
	}
}

void AVeggleGameState::SetPegboard(AVegglePegboard* InPegboard)
{
	Pegboard = InPegboard;
}

void AVeggleGameState::SetWalkerSpawner(AVeggleWalkerSpawner* InWalkerSpawner)
{
	WalkerSpawner = InWalkerSpawner;
}

void AVeggleGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ActiveDish.IsValid() && CurrentDishTimeRemaining > 0.0f)
	{
		CurrentDishTimeRemaining -= DeltaSeconds;
		OnUpdateDishTimer.Broadcast(CurrentDishTimeRemaining, ActiveDish.Get()->GetRecipe()->MaxPreparationTime, ActiveDish.Get()->GetRecipe()->OptimalPreparationTime);

		if (CurrentDishTimeRemaining <= 0.0f)
		{
			OnDishTimerExpired.Broadcast(ActiveDish.Get(), EDishCompletionState::TimeOut);
		}
	}
}

void AVeggleGameState::StartPregameCountdown(float Duration)
{
	PregameCountdownTimeRemaining = Duration;

	GetWorld()->GetTimerManager().SetTimer(PregameCountdownTimerHandle, this, &AVeggleGameState::UpdatePregameCountdown, 1.0f, true);

	OnPregameCountdownStarted.Broadcast();
	OnPregameCountdownTick.Broadcast(PregameCountdownTimeRemaining);
}

void AVeggleGameState::UpdatePregameCountdown()
{
	PregameCountdownTimeRemaining--;
	OnPregameCountdownTick.Broadcast(PregameCountdownTimeRemaining);
	
	if (PregameCountdownTimeRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(PregameCountdownTimerHandle);
		OnPregameCountdownFinished.Broadcast();
	}
}
