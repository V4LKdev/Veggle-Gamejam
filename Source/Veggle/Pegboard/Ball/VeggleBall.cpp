// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#include "VeggleBall.h"

#include "AbilitySystemComponent.h"
#include "Veggle/VeggleGameUtils.h"
#include "Veggle/AbilitySystem/VeggleVeggieAttributeSet.h"
#include "Veggle/Core/VeggleEventSubsystem.h"
#include "Veggle/Events/VeggleEventPayloads.h"
#include "Veggle/Pegboard/Board/VegglePegboard.h"
#include "Veggle/Pegboard/Peg/VegglePegData.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleBall)

AVeggleBall::AVeggleBall()
{
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>("BallMesh");
	SetRootComponent(BallMesh);
	BallMesh->SetCollisionProfileName(TEXT("PegBall"));
	BallMesh->SetNotifyRigidBodyCollision(true);
	BallMesh->SetSimulatePhysics(false);
	BallMesh->SetGenerateOverlapEvents(true);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	AttributeSet = CreateDefaultSubobject<UVeggleVeggieAttributeSet>("AttributeSet");
}

void AVeggleBall::LaunchBall(const FVector& ImpulseDirection)
{
	BallMesh->SetSimulatePhysics(true);
	BallMesh->AddImpulse(ImpulseDirection * BallData->LaunchStrength, NAME_None, true);
	SetActorTickEnabled(true);
}




void AVeggleBall::BeginPlay()
{
	Super::BeginPlay();

	if (!BallData || !BaseBallMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("AVeggleBall::BeginPlay: BallData or BaseBallMaterial is not set!"));
		return;
	}

	BallMaterialInstance = BallMesh->CreateDynamicMaterialInstance(0, BaseBallMaterial);
	BallMaterialInstance->SetTextureParameterValue(TEXT("BaseTexture"), BallData->BallTexture);

	SetActorTickEnabled(false);

	BallMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnBallHit);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	AbilitySystemComponent->AddLooseGameplayTag(BallData->VeggieType);

	AttributeSet->OnAttributeChanged.AddDynamic(this, &ThisClass::OnAttributeChanged);
}

void AVeggleBall::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	// when the ball is slow / still, disable touching pegs / pegs in a radius around the ball after some time
	if (BallMesh->IsSimulatingPhysics() && BallMesh->GetPhysicsLinearVelocity().SizeSquared() < MaxVelocitySquaredToIdle)
	{
		// Check if timer is already running
		if (GetWorld()->GetTimerManager().IsTimerActive(IdleTimerHandle))
		{
			return;
		}
		
		// Start timer to disable the ball after a short delay if the velocity doesn't go over again
		GetWorld()->GetTimerManager().SetTimer(IdleTimerHandle, [this]()
		{
					// get currently touching pegs with object query searching for "Peg" custom object type
					TArray<FHitResult> HitResults;
					const FCollisionShape SphereShape = FCollisionShape::MakeSphere(IdleDisablePegsRadius);
					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(this);
					QueryParams.bTraceComplex = false;
					if (GetWorld()->SweepMultiByObjectType(HitResults, GetActorLocation(), GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams(ECC_GameTraceChannel2), SphereShape, QueryParams))
					{
						for (const FHitResult& Hit : HitResults)
						{
							if (AVegglePegboard* Pegboard = Cast<AVegglePegboard>(Hit.GetActor()))
							{
								Pegboard->RemoveStuckPeg(Hit.Item);
							}
						}
					}
			
					// DrawDebugSphere(GetWorld(), GetActorLocation(), IdleDisablePegsRadius, 12, FColor::Red, false, 0.1f);
		},
		IdleTimeBeforeReset,
		false);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(IdleTimerHandle);
	}
}

void AVeggleBall::OnAttributeChanged_Implementation(const FGameplayTag& AttributeTag, float NewValue)
{
}

void AVeggleBall::OnBallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	const UVegglePegData* HitPegData = nullptr;
	bool bWasPegHit = false;
	
	ON_SCOPE_EXIT 
	{
		// Update the dynamic material instance of the ball mesh to reflect the effects, or other visual elements and sounds
		BP_OnBallHit(HitPegData, bWasPegHit);
	};


	// Instead of the Peg actor we now check for the pegboards hism
	AVegglePegboard* Pegboard = Cast<AVegglePegboard>(OtherActor);
	if (!IsValid(Pegboard))
	{
		// TODO: handle special wall logic here
		return;
	}
	
	HitPegData = Pegboard->GetDataFromInstanceIndex(Hit.Item);
	if (!IsValid(HitPegData) || !Pegboard->IsPegEnabled(Hit.Item))
	{
		// If the peg is not valid or not enabled, do not process the hit.
		return;
	}

	bWasPegHit = true;
	
	// Call hit logic on the pegboard to handle the reaction
	Pegboard->OnActivePegHit(Hit.Item);
	

	// apply all effects to the ball based on the hit peg data
	for (const TSubclassOf<UGameplayEffect>& EffectClass : HitPegData->PegEffects)
	{
		if (EffectClass)
		{
			ApplyEffectToSelf(Pegboard, EffectClass);
		}
	}

	// check if it's the last peg, if so, apply a bonus score to the ball
	const bool bBoardCleared = Pegboard->IsPegboardCleared();
	if (bBoardCleared)
	{
		// Apply board cleared effect to the ball
		if (BoardClearedEffect)
		{
			ApplyEffectToSelf(Pegboard, BoardClearedEffect);
		}
	}

	// Debug log the Special Multiplier
	float SpecialMultiplier = AttributeSet->GetSpecialMultiplier();

	// Broadcast the ball hit event
	FVeggieHitPegPayload Payload;
	Payload.PegType = HitPegData;
	Payload.bBoardCleared = bBoardCleared;
	Payload.BallASC = AbilitySystemComponent;
	UVeggleGameUtils::GetVeggleEventSubsystem(this)->OnVeggieHitPeg.Broadcast(Payload);

	// Log the fillerMult
	UE_LOG(LogTemp, Log, TEXT("Filler mult is: %f"), AbilitySystemComponent->GetNumericAttribute(UVeggleVeggieAttributeSet::GetFillerMultiplierAttribute()));
}

void AVeggleBall::ApplyEffectToSelf(const AVegglePegboard* Pegboard, const TSubclassOf<UGameplayEffect>& EffectClass) const
{
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(Pegboard);
		
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.0f, ContextHandle);
		
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
