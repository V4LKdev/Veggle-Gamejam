// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VeggleGameInstance.generated.h"

class UVeggleLevelData;
/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UVeggleGameInstance();

	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable, Category = "LevelData")
	void SetSelectedLevelData(UVeggleLevelData* LevelData) { SelectedLevelData = LevelData; }
	UFUNCTION(BlueprintCallable, Category = "LevelData")
	UVeggleLevelData* GetSelectedLevelData() const { return SelectedLevelData; }
	UFUNCTION(BlueprintCallable, Category = "LevelData")
	FString GetSaveGameSlotName() const { return SaveGameSlotName; }
	
private:
	UPROPERTY(EditAnywhere, Category = "LevelData")
	TObjectPtr<UVeggleLevelData> SelectedLevelData;
	
	UPROPERTY(EditAnywhere, Category = "SaveGame")
	FString SaveGameSlotName = TEXT("VeggleSaveGame");
};
