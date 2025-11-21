// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "VeggleViewportClient.generated.h"


UENUM()
namespace ECustomSplitScreenType
{
	enum Type
	{
		None,
		TwoPlayer_Custom = ESplitScreenType::SplitTypeCount,
		SplitTypeCount
	};
}


/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	UVeggleViewportClient(const FObjectInitializer& ObjectInitializer);

	virtual void UpdateActiveSplitscreenType() override;
};
