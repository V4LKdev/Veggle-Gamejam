// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VegglePlayerController.h"

#include "VeggleGameMode.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "VeggleEventSubsystem.h"
#include "VeggleGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/Events/VeggleEventPayloads.h"
#include "Veggle/Pegboard/VeggleLauncherPawn.h"
#include "Veggle/WalkingVeggies/VeggleWalker.h"
#include "Veggle/WalkingVeggies/VeggleWalkingVeggieData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VegglePlayerController)

AVegglePlayerController::AVegglePlayerController()
{
	bEnableClickEvents = false;
}

void AVegglePlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);

	if (IsPrimaryPlayer())
	{
		// Launcher pawn is placed in level and set to autopossess p0
		LauncherPawn = Cast<AVeggleLauncherPawn>(GetPawn());

		UVeggleGameUtils::GetVeggleGameState(this)->OnPostgameStarted.AddDynamic(this, &ThisClass::OnGameEnded);
	}
}

void AVegglePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// We only bind and handle input on the primary player controller
	if (!IsPrimaryPlayer())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(IMC, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	if (!Input)
	{
		return;
	}
	
	Input->BindAction(IA_Select, ETriggerEvent::Triggered, this, &ThisClass::OnSelect);
}

void AVegglePlayerController::OnSelect(const FInputActionValue& Value)
{
	if (!IsPrimaryPlayer() || !IsValid(LauncherPawn))
	{
		return;
	}

	// Either Fire loaded ball or try reloading the ball with a selected veggie
	if (LauncherPawn->IsLoaded())
	{
		LauncherPawn->LaunchBall();
	}
	else
	{
		const APlayerController* OtherPC = UGameplayStatics::GetPlayerController(GetWorld(), 1);

		if (OtherPC)
		{
			FHitResult Hit;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));

			// Perform the trace under the other local player's cursor since the veggies are in the other viewport
			OtherPC->GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit);

			if (AActor* HitActor = Hit.GetActor())
			{
				if (AVeggleWalker* ClickedVeggie = Cast<AVeggleWalker>(HitActor))
				{
					LauncherPawn->ReloadBall(ClickedVeggie->GetVeggieType()->BallData);
					ClickedVeggie->Destroy();

					// Broadcast the veggie loaded message
					FVeggieLoadedPayload VeggieLoadedPayload;
					VeggieLoadedPayload.VeggieData = ClickedVeggie->GetVeggieType();
					UVeggleGameUtils::GetVeggleEventSubsystem(this)->OnVeggieLoaded.Broadcast(VeggieLoadedPayload);
				}
			}
		}
	}
}

void AVegglePlayerController::OnGameEnded()
{
	// Unmap the input context and disable the tick hover
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		Subsystem->ClearAllMappings();
	}
	bCanHighlight = false;
}


void AVegglePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bCanHighlight)
	{
		return;
	}

	HighlightObjectsUnderCursor();
}

void AVegglePlayerController::HighlightObjectsUnderCursor() const
{
	// the highlighting logic is only relevant for the second player controllers viewport
	// TODO: we could implement a check that the highlighting only occurs when the launcher doesnt have a ball loaded, This will require some routing tho
	if (IsPrimaryPlayer())
	{
		return;
	}

	static AActor* LastActor = nullptr;

	FHitResult Hit;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1)); // This is a custom trace channel for Walking Veggies
	GetHitResultUnderCursorForObjects(ObjectTypes, false, Hit);

	AActor* CurrentActor = Hit.GetActor();
	
	if (CurrentActor)
	{
		if (LastActor != CurrentActor)
		{
			// Actor under cursor changed
			if (LastActor)
			{
				HighlightActor(LastActor, false);
			}
			HighlightActor(CurrentActor, true);
		}
	}
	else
	{
		if (LastActor)
		{
			HighlightActor(LastActor, false);
		}
	}

	LastActor = CurrentActor;
}

void AVegglePlayerController::HighlightActor(const AActor* Actor, const bool bHighlight) const
{
	// for simplicity, just loop over all primitive components of the actor and set the custom depth on them
	TArray<UActorComponent*> PrimitiveComponents = Actor->K2_GetComponentsByClass(UPrimitiveComponent::StaticClass());

	if (PrimitiveComponents.Num() == 0)
	{
		return;
	}

	// foreach component activate or deactivate custom depth
	for (UActorComponent* Component : PrimitiveComponents)
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
		{
			if (bHighlight)
			{
				PrimComp->SetRenderCustomDepth(true);
				PrimComp->SetCustomDepthStencilValue(OutlineDepthStencilValue);
			}
			else
			{
				PrimComp->SetRenderCustomDepth(false);
			}
		}
	}
}
