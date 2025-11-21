// Fill out your copyright notice in the Description page of Project Settings.


#include "VeggleWalkerSpawner.h"

#include "VeggleWalker.h"
#include "VeggleWalkingVeggieData.h"
#include "Kismet/GameplayStatics.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/Core/VeggleGameState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleWalkerSpawner)

AVeggleWalkerSpawner::AVeggleWalkerSpawner()
	:	SpawnLocationA (FVector::ZeroVector),
		SpawnLocationB(FVector(0.0f, 100.0f, 0.0f)),
		VeggieWalkerClass(nullptr),
		MaxVeggies(10),
		StartSpawnInterval(2.0f),
		EndSpawnInterval(2.0f),
		CurrentSpawnInterval(2.0f),
		RampUpTime(20.f)
{
 	PrimaryActorTick.bCanEverTick = true;
}

void AVeggleWalkerSpawner::InitSpawner(const TMap<UVeggleWalkingVeggieData*, float>& InVeggieWalkerTypes, const int32 InMaxVeggies, const float InStartSpawnInterval, const float InEndSpawnInterval, const float InRampUpTime)
{
	VeggieWalkerTypes = InVeggieWalkerTypes;
	MaxVeggies = InMaxVeggies;
	StartSpawnInterval = InStartSpawnInterval;
	EndSpawnInterval = InEndSpawnInterval;
	RampUpTime = InRampUpTime;
	
	CurrentSpawnInterval = StartSpawnInterval;
}

void AVeggleWalkerSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SpawnTimer += DeltaTime;
	if (SpawnTimer >= CurrentSpawnInterval && SpawnedVeggies.Num() < MaxVeggies)
	{
		TrySpawnVeggieWalker();
		SpawnTimer = 0.0f;
	}

	RampSpawnInterval(DeltaTime);
}

void AVeggleWalkerSpawner::RampSpawnInterval(float DeltaTime)
{
	if (RampUpTime <= 0.0f || CurrentSpawnInterval == EndSpawnInterval)
	{
		return;
	}

	ElapsedRampTime += DeltaTime;
	const float Alpha = FMath::Clamp(ElapsedRampTime / RampUpTime, 0.0f, 1.0f);
	CurrentSpawnInterval = FMath::Lerp(StartSpawnInterval, EndSpawnInterval, Alpha);
}


void AVeggleWalkerSpawner::TrySpawnVeggieWalker()
{

	UVeggleWalkingVeggieData* VeggieType = GetRandomVeggieWalkerTypeWeighted();
	if (!IsValid(VeggieType))
	{
		return;
	}
	
	SpawnVeggieWalker_Internal(VeggieType);
}



void AVeggleWalkerSpawner::SpawnVeggieWalker_Internal(UVeggleWalkingVeggieData* VeggieType)
{

	const FTransform SpawnTransform = FTransform(
		FQuat::Identity,
		GetRandomSpawnLocation(),
		FVector(1.0f)
	);


	AActor* SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(
		VeggieWalkerClass,
		SpawnTransform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding
	);

	if (!IsValid(SpawnedActor)) return;

	AVeggleWalker* VeggieWalker = Cast<AVeggleWalker>(SpawnedActor);
	
	if (!IsValid(VeggieWalker))
	{
		return;
	}
	
	VeggieWalker->InitVeggieWalker(VeggieType, this);
	
	SpawnedVeggies.Add(SpawnedActor);
	
	UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);
}

FVector AVeggleWalkerSpawner::GetRandomSpawnLocation() const
{
	FVector OutLocation = FMath::Lerp(SpawnLocationA, SpawnLocationB, FMath::FRand());
	OutLocation.X += FMath::FRandRange(-SpawnVarianceX, SpawnVarianceX);
	OutLocation = GetTransform().TransformPosition(OutLocation);
	
	return OutLocation;
}

UVeggleWalkingVeggieData* AVeggleWalkerSpawner::GetRandomVeggieWalkerTypeWeighted() const
{
	if (VeggieWalkerTypes.Num() == 0)
	{
		return nullptr;
	}

	float TotalWeight = 0.0f;
	for (const TPair<UVeggleWalkingVeggieData*, float>& Elem : VeggieWalkerTypes)
	{
		TotalWeight += Elem.Value;
	}

	if (TotalWeight <= 0.0f)
	{
		return nullptr;
	}

	float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);
	float AccumWeight = 0.0f;

	for (const TPair<UVeggleWalkingVeggieData*, float>& Elem : VeggieWalkerTypes)
	{
		AccumWeight += Elem.Value;
		if (RandomWeight <= AccumWeight)
		{
			return Elem.Key;
		}
	}
	return nullptr;
}

void AVeggleWalkerSpawner::OnVeggieDestroyed(AActor* DestroyedActor)
{
	if (!IsValid(DestroyedActor) || !SpawnedVeggies.Contains(DestroyedActor) || !GetWorld())
	{
		return;
	}

	if (SpawnedVeggies.Num() >= MaxVeggies)
	{
		SpawnTimer = 0.f; // Reset the spawn timer to prevent instant respawn
	}

	SpawnedVeggies.Remove(DestroyedActor);
}

void AVeggleWalkerSpawner::StartSpawner()
{
	SetActorTickEnabled(true);
}

void AVeggleWalkerSpawner::StopSpawner()
{
	// easy solution
	SetActorTickEnabled(false);
}

void AVeggleWalkerSpawner::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
}
