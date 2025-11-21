// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VegglePegboard.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Veggle/Pegboard/Peg/VegglePegData.h"
#include "Veggle/Pegboard/Peg/VegglePegUtility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VegglePegboard)

AVegglePegboard::AVegglePegboard()
{
	PrimaryActorTick.bCanEverTick = false;

	CirclePegInstances = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("CirclePegInstances"));
	SetRootComponent(CirclePegInstances);
	CirclePegInstances->SetCollisionProfileName(TEXT("Peg"));
	CirclePegInstances->SetNotifyRigidBodyCollision(true);
	CirclePegInstances->CastShadow = false;
	CirclePegInstances->SetNumCustomDataFloats(3);
}

void AVegglePegboard::GenerateBoard()
{
	RegeneratePegboard();
}

void AVegglePegboard::ClearBoard()
{
	CirclePegInstances->ClearInstances();
	PegDataMap.Empty();
	DeactivatedPegs.Empty();
	PendingInstances.Empty();
}


UVegglePegData* AVegglePegboard::GetDataFromInstanceIndex(const int32 InstanceIndex) const
{
	return PegDataMap.FindRef(InstanceIndex);
}


void AVegglePegboard::OnActivePegHit(const int32 InstanceIndex)
{
	if (!CirclePegInstances->IsValidInstance(InstanceIndex))
	{
		return;
	}
	
	const UVegglePegData* PegData = PegDataMap.FindRef(InstanceIndex);
	if (!IsValid(PegData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Peg data not found for instance index %d"), InstanceIndex);
		return;
	}

	FTimerHandle HitTimerHandle;
	FTimerDelegate HitDelegate;
	HitDelegate.BindLambda([this, InstanceIndex, PegData]()
	{
		HandlePegHit(InstanceIndex, PegData);
	});
	GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, HitDelegate, PegData->PegHitDelay, false);
}

void AVegglePegboard::HandlePegHit(const int32 HitInstance, const UVegglePegData* PegData)
{
	switch (PegData->HitBehaviour)
	{
	case PHB_Deactivate:
		{
			SetPegEnabledValue(HitInstance, 0.0f);
			SetPegTexture(HitInstance, static_cast<uint8>(PegData->PegTexture), false);
			ActivePegs.Remove(HitInstance);
			DeactivatedPegs.Add(HitInstance);
		}
		break;
	case PHB_DestroyImmediate:
		{
			HidePeg(HitInstance);
		}
		break;
	case PHB_DestroyAfterCount:
		{
			int32 HitCount = GetPerInstanceCustomDataValue(HitInstance, 2);
			SetPegEnabledValue(HitInstance, HitCount - 1.0f);
			
			if (HitCount >= PegData->DestroyAfterCount)
			{
				HidePeg(HitInstance);
			}
		}
		break;
	case PHB_None:
		break;
	default:
		break;
	}

	if (bIsPegboardCleared)
	{
		// TODO: some visuals or message send?
		RegeneratePegboard();
	}
}

void AVegglePegboard::RemoveStuckPeg(const int32 InstanceToDisable)
{
	if (CirclePegInstances->IsValidInstance(InstanceToDisable))
	{
		HidePeg(InstanceToDisable);
	}
}

void AVegglePegboard::CleanupDeactivatedPegs()
{
	if (DeactivatedPegs.Num() <= 0)
	{
		return;
	}

	for (int32 i = 0; i < DeactivatedPegs.Num(); i++)
	{
		HidePeg(DeactivatedPegs[i]);
	}

	DeactivatedPegs.Empty();
}

bool AVegglePegboard::IsPegboardCleared()
{
	if (ActivePegs.Num() <= 1) // If there's only one peg left, we consider the board cleared because the last pegs hit is processed delayed
	{
		bIsPegboardCleared = true;
		return true;
	}
	return false;
}

void AVegglePegboard::HidePeg(const int32 HitInstance)
{
	SetPegEnabledValue(HitInstance, 0.0f);
	const FTransform HiddenTransform(FQuat::Identity, FVector::ZeroVector, FVector::ZeroVector);
	CirclePegInstances->UpdateInstanceTransform(HitInstance, HiddenTransform, true, true, true);
	ActivePegs.Remove(HitInstance);
}

bool AVegglePegboard::RegeneratePegboard()
{
	if (!IsValid(CirclePegInstances) || !IsValid(LayoutData) || !IsValid(PegMesh) || !IsValid(PegMaterial))
	{
		UE_LOG(LogTemp, Error, TEXT("Pegboard is not properly set up. Missing components or data table."));
		return false;
	}
	
	ClearBoard();
	
	CirclePegInstances->SetStaticMesh(PegMesh);
	CirclePegInstances->SetMaterial(0, PegMaterial);

	for (const TPair<FName, uint8*>& Row : LayoutData->GetRowMap())
	{
		const FVegglePegInstanceData* PegInstanceData = reinterpret_cast<FVegglePegInstanceData*>(Row.Value);
		if (PegInstanceData)
		{
			PendingInstances.Add(PegInstanceData);
		}
	}
	
	NextPegToSpawn = 0;
	bIsSpawningBoard = true;
	SpawnNextPeg();
	return true;
	
#if 0
	// go over each row in the data table and extract the row struct (FVegglePegInstanceData)
	for (const TPair<FName, uint8*>& Row  : LayoutData->GetRowMap())
	{
		const FVegglePegInstanceData* PegInstanceData = reinterpret_cast<FVegglePegInstanceData*>(Row.Value);
		if (PegInstanceData)
		{
			int32 InstanceIndex = CirclePegInstances->AddInstance(PegInstanceData->PegTransform, true);
			PegDataMap.Add(InstanceIndex, PegInstanceData->PegData);

			uint8 TextureValue = static_cast<uint8>(PegInstanceData->PegData->PegTexture);
			SetPegTexture(InstanceIndex, TextureValue, true);

			// Set the 3rd custom data value to indicate whether the peg is enabled or not, each hit decrements the value by one, that's why "destroy after count" gets assigned a higher value
			const float EnabledValue = PegInstanceData->PegData->HitBehaviour == PHB_DestroyAfterCount ? PegInstanceData->PegData->DestroyAfterCount : 1.0f;

			CirclePegInstances->SetCustomDataValue(InstanceIndex, 2, EnabledValue);

			ActivePegs.Add(InstanceIndex);
		}
	}

	return true;
#endif
}


float AVegglePegboard::GetPerInstanceCustomDataValue(const int32 InstanceIndex, const int32 ValueIndex) const
{
	const int32 DataIndex = InstanceIndex * CirclePegInstances->NumCustomDataFloats + ValueIndex;
	if (!CirclePegInstances->PerInstanceSMCustomData.IsValidIndex(DataIndex))
	{
		return -1.0f;
	}
	return CirclePegInstances->PerInstanceSMCustomData[DataIndex];
}

void AVegglePegboard::SetPegEnabledValue(const int32 InstanceIndex, const float Value) const
{
	CirclePegInstances->SetCustomDataValue(InstanceIndex, 2, Value);
}

void AVegglePegboard::SetPegTexture(int32 InstanceIndex, const uint8 TextureIndex, const bool bActive) const
{
	CirclePegInstances->SetCustomDataValue(InstanceIndex, 0, TextureIndex);
	CirclePegInstances->SetCustomDataValue(InstanceIndex, 1, bActive ? 1.0f : 0.0f);
}

void AVegglePegboard::SpawnNextPeg()
{
	if (NextPegToSpawn >= PendingInstances.Num())
	{
		bIsSpawningBoard = false;
		OnBoardSpawned();
		return;
	}

	const FVegglePegInstanceData* PegInstanceData = PendingInstances[NextPegToSpawn];
	int32 InstanceIndex = CirclePegInstances->AddInstance(PegInstanceData->PegTransform, true);
	PegDataMap.Add(InstanceIndex, PegInstanceData->PegData);
	
	SetPegTexture(InstanceIndex, static_cast<uint8>(PegInstanceData->PegData->PegTexture), true);
	float EnabledValue = PegInstanceData->PegData->HitBehaviour == PHB_DestroyAfterCount ? PegInstanceData->PegData->DestroyAfterCount : 1.0f;
	CirclePegInstances->SetCustomDataValue(InstanceIndex, 2, EnabledValue);
	
	ActivePegs.Add(InstanceIndex);

	NextPegToSpawn++;
	FTimerHandle SpawnTimerHandle;
	FTimerDelegate SpawnDelegate;
	SpawnDelegate.BindLambda([this]()
	{
		SpawnNextPeg();
	});
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, SpawnDelegate, PegSpawnDelay, false);

	if (IsValid(PegSpawnSound))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), PegSpawnSound);
	}
}

void AVegglePegboard::OnBoardSpawned()
{
}
