// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleBallCollector.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Components/BoxComponent.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/Core/VeggleGameState.h"
#include "Veggle/Dish/VeggleDish.h"
#include "Veggle/Pegboard/Ball/VeggleBall.h"
#include "Veggle/Pegboard/Board/VegglePegboard.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleBallCollector)

AVeggleBallCollector::AVeggleBallCollector()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CollectorCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollectorCollisionBox"));
	SetRootComponent(CollectorCollisionBox);
	CollectorCollisionBox->SetCollisionProfileName(TEXT("OverlapOnlyBall"));
	CollectorCollisionBox->SetGenerateOverlapEvents(true);
	CollectorCollisionBox->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
}

void AVeggleBallCollector::BeginPlay()
{
	Super::BeginPlay();

	CollectorCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AVeggleBallCollector::OnBallOverlap);
}

void AVeggleBallCollector::OnBallOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
	{
		return;
	}
	
	// A few things to do here:
	// 1. check if the OtherActor is a valid AVeggleBall
	// 2. check if the game state has an active dish
	// 3. if so, apply the accumulation gameplay effect to the active dish's AbilitySystemComponent
	// 4. TODO: destroy the ball and spawn in visual vegetable for the dish dropper
	// 5. Cleanup the pegboard

	// 1
	AVeggleBall* Ball = Cast<AVeggleBall>(OtherActor);
	const AVeggleGameState* GameState = UVeggleGameUtils::GetVeggleGameState(this);
	
	if (!IsValid(GameState) || !IsValid(Ball))
	{
		OtherActor->Destroy(); // safe fallback 
		return;
	}

	// 2
	AVeggleDish* ActiveDish = GameState->GetActiveDish();
	
	if (!IsValid(ActiveDish))
	{
		Ball->Destroy(); // Possibly implement ball specific destroy logic later
		return;
	}

	// 3
	UAbilitySystemComponent* SourceASC = Ball->GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ActiveDish->GetAbilitySystemComponent();

	if (SourceASC && TargetASC)
	{
		// Apply the accumulation gameplay effect.
		TSubclassOf<UGameplayEffect> AccumulationEffect = Ball->GetAccumulationEffect(); 
		if (AccumulationEffect)
		{
			FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
			ContextHandle.AddSourceObject(Ball);
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(AccumulationEffect, 1.0f, ContextHandle);
            
			if (SpecHandle.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
	
	// 4
	Ball->Destroy();
	
	// 5
	GameState->GetPegboard()->CleanupDeactivatedPegs();
}
