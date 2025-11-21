// Fill out your copyright notice in the Description page of Project Settings.


#include "VeggleWalker.h"

#include "VeggleWalkerSpawner.h"
#include "VeggleWalkingVeggieData.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/Core/VeggleGameState.h"
#include "Veggle/Core/VegglePlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleWalker)


AVeggleWalker::AVeggleWalker()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	RootComponent = CapsuleCollision;
	CapsuleCollision->SetCollisionProfileName(TEXT("IgnoreWalkingVeg"));
	CapsuleCollision->SetSimulatePhysics(true);
	CapsuleCollision->bIgnoreRadialForce = true;
	CapsuleCollision->bIgnoreRadialImpulse = true;
	CapsuleCollision->SetNotifyRigidBodyCollision(true);
	CapsuleCollision->BodyInstance.bLockRotation = true;

	VeggieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VeggieMesh"));
	VeggieMesh->SetupAttachment(RootComponent);
	VeggieMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -CapsuleCollision->GetScaledCapsuleHalfHeight()));
	VeggieMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AVeggleWalker::InitVeggieWalker(UVeggleWalkingVeggieData* NewVeggieType, AVeggleWalkerSpawner* Spawner)
{
	if (!IsValid(NewVeggieType) || !IsValid(Spawner))
	{
		return;
	}
	
	SpawnerRef = Spawner;

	VeggieType = NewVeggieType;
	
	// Setup
	VeggieMesh->SetStaticMesh(VeggieType->VeggieMesh);
	CapsuleCollision->SetPhysMaterialOverride(VeggieType->PhysicsMaterial);
}

void AVeggleWalker::BeginPlay()
{
	Super::BeginPlay();

	checkf(VeggieType, TEXT("VeggieType should be set before BeginPlay!"));
	checkf(SpawnerRef.IsValid(), TEXT("SpawnerRef should be set before BeginPlay!"));

	CapsuleCollision->SetConstraintMode(EDOFMode::Default);
	
	CapsuleCollision->OnComponentHit.AddDynamic(this, &ThisClass::OnVeggieHit);

	// set the game ended bool
	UVeggleGameUtils::GetVeggleGameState(this)->OnPostgameStarted.AddDynamic(this, &ThisClass::OnGameEnded);
}

void AVeggleWalker::Destroyed()
{
	if (SpawnerRef.IsValid())
	{
		SpawnerRef->OnVeggieDestroyed(this);
	}
	Super::Destroyed();
}

void AVeggleWalker::OnVeggieHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnVeggieHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

bool AVeggleWalker::GetRandomHopLeft() const
{
	// return random bool, but if the game has ended, always return true
	if (bGameEnded)
	{
		return true;
	}
	// Slightly more likely to hop left
	return UKismetMathLibrary::RandomBoolWithWeight(0.6f);
}

float AVeggleWalker::GetRandomTimeToNextHop() const
{
	if (bGameEnded)
	{
		return 0.1f;
	}
	return FMath::RandRange(VeggieType->HopCooldownMin, VeggieType->HopCooldownMax);
}

void AVeggleWalker::OnGameEnded()
{
	bGameEnded = true;
	BP_OnGameEnded();
}
