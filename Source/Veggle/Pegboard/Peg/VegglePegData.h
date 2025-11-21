// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "VegglePegData.generated.h"


class UGameplayEffect;


UENUM(BlueprintType)
enum EPegHitBehaviour : uint8
{
	PHB_None UMETA(DisplayName = "None"),
	PHB_Deactivate UMETA(DisplayName = "Deactivate"),
	PHB_DestroyImmediate UMETA(DisplayName = "Destroy Immediate"),
	PHB_DestroyAfterCount UMETA(DisplayName = "Destroy After Count"),

	PHB_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPegTexture : uint8
{
	PT_Normal = 0 UMETA(DisplayName = "Normal"),
	PT_Salt = 1 UMETA(DisplayName = "Salt"),
	PT_Pepper = 2 UMETA(DisplayName = "Pepper"),
	PT_Butter = 3 UMETA(DisplayName = "Butter"),
	PT_Heat = 4 UMETA(DisplayName = "Heat"),
	PT_Wildcard = 5 UMETA(DisplayName = "Wildcard")
};


UCLASS()
class VEGGLE_API UVegglePegData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PegData", meta = (Categories = "Veggle.Flavour"));
	FGameplayTag PegTypeTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PegEffect")
	TArray<TSubclassOf<UGameplayEffect>> PegEffects;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PegBehaviour")
	TEnumAsByte<EPegHitBehaviour> HitBehaviour = PHB_None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PegBehaviour")
	int32 DestroyAfterCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PegBehaviour", meta = (ToolTip = "Delay before the peg hit logic gets called"))
	float PegHitDelay = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PegVisuals")
	EPegTexture PegTexture;
};
