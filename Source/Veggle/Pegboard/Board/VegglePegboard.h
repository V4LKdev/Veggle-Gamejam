// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VegglePegboard.generated.h"

struct FVegglePegInstanceData;
class AVeggleGameState;
class UVegglePegData;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class VEGGLE_API AVegglePegboard : public AActor
{
	GENERATED_BODY()

public:
	AVegglePegboard();
	UFUNCTION(CallInEditor, Category = "Pegboard")
	void GenerateBoard();
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Pegboard")
	void ClearBoard();

	bool RegeneratePegboard();
	UVegglePegData* GetDataFromInstanceIndex(int32 InstanceIndex) const;
	bool IsPegEnabled(const int32 InstanceIndex) const { return GetPerInstanceCustomDataValue(InstanceIndex, 2) > 0.0f; }
	void SetLayoutData(UDataTable* NewLayoutData) { LayoutData = NewLayoutData; }
	void OnActivePegHit(const int32 InstanceIndex);
	void RemoveStuckPeg(int32 InstanceToDisable);
	void CleanupDeactivatedPegs();
	bool IsPegboardCleared();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pegboard")
	TObjectPtr<UStaticMesh> PegMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pegboard")
	TObjectPtr<UMaterialInterface> PegMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pegboard", meta=(DisplayName="Default Layout Data"))
	TObjectPtr<UDataTable> LayoutData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pegboard")
	TObjectPtr<USoundBase> PegSpawnSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pegboard")
	float PegSpawnDelay = 0.1f;

private:
	void HandlePegHit(int32 HitInstance, const UVegglePegData* PegData);
	void HidePeg(int32 HitInstance);
	float GetPerInstanceCustomDataValue(int32 InstanceIndex, int32 ValueIndex) const;
	void SetPegEnabledValue(int32 InstanceIndex, float Value) const;
	void SetPegTexture(int32 InstanceIndex, const uint8 TextureIndex, const bool bActive) const;
	void SpawnNextPeg();
	void OnBoardSpawned();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> CirclePegInstances;
	TMap<int32, TObjectPtr<UVegglePegData>> PegDataMap;
	TArray<int32> DeactivatedPegs;
	TArray<int32> ActivePegs;
	TArray <const FVegglePegInstanceData*> PendingInstances;

	bool bIsPegboardCleared = false;
	bool bIsSpawningBoard = false;
	int32 NextPegToSpawn = 0;
};
