// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#include "VeggleGameMode.h"
#include "VeggleGameInstance.h"
#include "VeggleGameState.h"
#include "Data/VeggleLevelData.h"
#include "Kismet/GameplayStatics.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/Dish/VeggleDish.h"
#include "Veggle/Dish/VeggleTargetPoint.h"
#include "Veggle/Events/VeggleEventPayloads.h"
#include "Veggle/Pegboard/Board/VegglePegboard.h"
#include "Veggle/WalkingVeggies/VeggleWalkerSpawner.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleGameMode)


AVeggleGameMode::AVeggleGameMode()
{
	GameStateClass = AVeggleGameState::StaticClass();
	bDelayedStart = true;
}

void AVeggleGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Get the veggle game instance and initialize the level data from it
	const UVeggleGameInstance* VeggleGameInstance = GetGameInstance<UVeggleGameInstance>();
	checkf(VeggleGameInstance, TEXT("VeggleGameInstanceSubsystem is not available. Make sure it is added to the game instance."));

	LevelData = VeggleGameInstance->GetSelectedLevelData();
	checkf (LevelData, TEXT("LevelData is not set. Make sure to set a selected level in the game instance subsystem."));
}

void AVeggleGameMode::InitGameState()
{
	Super::InitGameState();

	if (GetVeggleGameState())
	{
		GetVeggleGameState()->OnDishTimerExpired.AddDynamic(this, &ThisClass::ProcessCompleteDish);
		GetVeggleGameState()->OnPregameCountdownStarted.AddDynamic(this, &ThisClass::OnPregameCountdownStarted);
		GetVeggleGameState()->OnPregameCountdownFinished.AddDynamic(this, &ThisClass::StartMatch);
		GetVeggleGameState()->LevelData = LevelData;
	}
}

void AVeggleGameMode::StartPlay()
{
	Super::StartPlay();

	UGameplayStatics::SetForceDisableSplitscreen(this, false);
	
	if (UGameplayStatics::GetPlayerControllerFromID(this, 1) == nullptr)
	{
		// Create a second player controller if there is only one player
		UGameplayStatics::CreatePlayer(this, 1, true);
	}

	// Set the Spawner and initialize it
	if (AVeggleWalkerSpawner* WalkerSpawner = Cast<AVeggleWalkerSpawner>(UGameplayStatics::GetActorOfClass(this, AVeggleWalkerSpawner::StaticClass())))
	{
		GetVeggleGameState()->SetWalkerSpawner(WalkerSpawner);
		WalkerSpawner->InitSpawner(
			LevelData->VeggieWalkerTypes,
			LevelData->MaxVeggiesOnTable,
			LevelData->VeggieWalkerStartSpawnInterval,
			LevelData->VeggieWalkerEndSpawnInterval,
			LevelData->RampUpTime
		);
	}

	DishSpawnPoint = UGameplayStatics::GetActorOfClass(this, AVeggleTargetPoint::StaticClass());
}

void AVeggleGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	// TODO: initiate logic for dialogue etc
}

void AVeggleGameMode::OnPregameCountdownStarted()
{
	GetVeggleGameState()->WalkerSpawner->StartSpawner();
	
	// TODO: have the pegs spawn in one by one
	// Set the pegboard and initialize it
	if (AVegglePegboard* Pegboard = Cast<AVegglePegboard>(UGameplayStatics::GetActorOfClass(this, AVegglePegboard::StaticClass())))
	{
		GetVeggleGameState()->SetPegboard(Pegboard);
		Pegboard->SetLayoutData(LevelData->PegboardPattern);
		Pegboard->RegeneratePegboard();
	}
}


// put the above refactoring logic in here
void AVeggleGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	
	SpawnNextDish();
}

void AVeggleGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// TODO: implement game end logic
	
	GetVeggleGameState()->WalkerSpawner->StopSpawner();
}

void AVeggleGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	UE_LOG(LogTemp, Log, TEXT("Match State Changed to: %s"), *GetMatchState().ToString());
}

void AVeggleGameMode::ProcessCompleteDish(AVeggleDish* Dish, const EDishCompletionState CompletionState)
{
	if (!Dish) return;
    
	// Calculate the final score from the dish.
	bool bIsPerfect = false;
	const float FinalScore = Dish->CalculateFinalScore(bIsPerfect);
	
	VeggleGameState->AddToTotalScore(FinalScore);
    
	// Send message of dish completion TBD
	FDishCompletedPayload Payload;
	Payload.bIsPerfect = bIsPerfect;
	Payload.FinishedRecipe = Dish->GetRecipe();
	Payload.bWasTimeOut = CompletionState == EDishCompletionState::TimeOut;
	
	UVeggleGameUtils::GetVeggleEventSubsystem(this)->OnDishCompleted.Broadcast(Payload);

	Dish->OnDishServed.AddDynamic(this, &ThisClass::OnDishServed);
	Dish->ServeDish();
	
	VeggleGameState->SetActiveDish(nullptr);
}

void AVeggleGameMode::OnDishServed()
{
	VeggleGameState->GetPegboard()->RegeneratePegboard();
	SpawnNextDish();
}

void AVeggleGameMode::SpawnNextDish()
{
	if (!GetVeggleGameState() || IsValid(GetVeggleGameState()->GetActiveDish()))
	{
		return;
	}
	
	// Check if there are more dishes left in the sequence.
	if (LevelData && LevelData->DishSequence.IsValidIndex(CurrentDishIndex) && IsValid(DishSpawnPoint))
	{
		UVeggleRecipeData* NewRecipe = LevelData->DishSequence[CurrentDishIndex];

		FVector SpawnLocation = DishSpawnPoint->GetActorLocation();
	
		AVeggleDish* NewDish = GetWorld()->SpawnActor<AVeggleDish>(DishClass, SpawnLocation, FRotator::ZeroRotator);
		
		NewDish->SetRecipe(NewRecipe);
		NewDish->OnDishCompleted.BindUObject(this, &ThisClass::ProcessCompleteDish);

		// 5. Tell the GameState about the new active dish.
		GetVeggleGameState()->SetActiveDish(NewDish);
		
		CurrentDishIndex++;
	}
	else
	{
		// set match state to ended
		EndMatch();
	}
}

AVeggleGameState* AVeggleGameMode::GetVeggleGameState()
{
	if (!VeggleGameState)
	{
		VeggleGameState = GetGameState<AVeggleGameState>();
	}
	return VeggleGameState;
}
