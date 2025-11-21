// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleLauncherPawn.h"

#include "Ball/VeggleBall.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Veggle/Core/VegglePlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleLauncherPawn)


AVeggleLauncherPawn::AVeggleLauncherPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	AimWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AimWidgetComponent"));
	AimWidgetComponent->SetupAttachment(RootComponent);
}

void AVeggleLauncherPawn::BeginPlay()
{
	Super::BeginPlay();
	CachedPlayerController = Cast<AVegglePlayerController>(GetController());

	AimWidgetComponent->SetVisibility(false);
}

void AVeggleLauncherPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLoaded())
	{
		Aim();
	}
}

void AVeggleLauncherPawn::Aim() const
{
	if (!IsValid(CachedPlayerController))
	{
		return;
	}

	FVector WorldLocation, WorldDirection;
	if (!CachedPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		return;
	}

	FVector TargetLocation = UKismetMathLibrary::ProjectPointOnToPlane(
		WorldLocation,
		GetActorLocation(),
		FVector::ForwardVector
	);
	
	if ((TargetLocation - GetActorLocation()).Size() < MinAimDistance)
	{
		return;
	}

	if (!bAllowAimAbove && TargetLocation.Z > GetActorLocation().Z)
	{
		TargetLocation.Z = GetActorLocation().Z;
	}

	FVector WidgetLocation = AimWidgetComponent->GetComponentLocation();
	FVector Forward = AimWidgetComponent->GetForwardVector();
	FVector ToTarget = (TargetLocation - WidgetLocation).GetSafeNormal();
	FVector ProjectedUp = FVector::VectorPlaneProject(ToTarget, Forward).GetSafeNormal();

	FRotator WidgetRotator = FRotationMatrix::MakeFromXZ(Forward, ProjectedUp).Rotator();
	WidgetRotator.Pitch = 0;
	WidgetRotator.Yaw = 180;

	AimWidgetComponent->SetWorldRotation(WidgetRotator);
}

void AVeggleLauncherPawn::LaunchBall()
{
	if (!bIsLoaded || !IsValid(CurrentBall))
	{
		return;
	}

	const FVector LaunchDirection = FVector::VectorPlaneProject(
		AimWidgetComponent->GetUpVector(),
		FVector(1, 0, 0));
	
	CurrentBall->LaunchBall(LaunchDirection);

	bIsLoaded = false;
	AimWidgetComponent->SetVisibility(false);
}

void AVeggleLauncherPawn::ReloadBall(UVeggleBallData* BallType)
{
	if (bIsLoaded || !IsValid(BallClass))
	{
		return;
	}

	CurrentBall = GetWorld()->SpawnActorDeferred<AVeggleBall>(
		BallClass,
		GetActorTransform(),
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	CurrentBall->SetBallData(BallType);
	
	UGameplayStatics::FinishSpawningActor(CurrentBall, GetActorTransform());

	bIsLoaded = true;
	AimWidgetComponent->SetVisibility(true);
}
