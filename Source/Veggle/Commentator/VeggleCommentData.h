// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "VeggleCommentData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct VEGGLE_API FVeggleCommentData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> VoiceLine;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Subtitle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> PortraitImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer ContextTags;
};