// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleViewportClient.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleViewportClient)

UVeggleViewportClient::UVeggleViewportClient(const FObjectInitializer& ObjectInitializer)
{
	// Make sure SplitscreenInfo array is big enough
	SplitscreenInfo.SetNum((int32)ECustomSplitScreenType::SplitTypeCount, EAllowShrinking::No);

	FSplitscreenData& TwoPlayer = SplitscreenInfo[static_cast<int32>(ECustomSplitScreenType::TwoPlayer_Custom)];
	
	// Player 0 (main camera) — bottom area, full width, remaining height, Camera aspect ratio is set to 1.5
	TwoPlayer.PlayerData.Add(FPerPlayerSplitscreenData(
		1.0f,   // full width
		0.8f,  // 80% height
		0.0f,   // start at left
		0.2f   // start just below top strip
	));

	// Player 1 (secondary camera) — top strip, full width, 20% height, Camera aspect ratio is set to 6
	TwoPlayer.PlayerData.Add(FPerPlayerSplitscreenData(
		1.0f,   // SizeX: full width
		0.20f,  // SizeY: 20% of screen height
		0.0f,   // OriginX: left edge
		0.0f    // OriginY: top edge
	));
}

void UVeggleViewportClient::UpdateActiveSplitscreenType()
{
	const int32 NumPlayers = GEngine->GetNumGamePlayers(GetWorld());
	if (NumPlayers == 2)
	{
		ActiveSplitscreenType = static_cast<ESplitScreenType::Type>(ECustomSplitScreenType::TwoPlayer_Custom);
	}
	else
	{
		Super::UpdateActiveSplitscreenType();
	}
}

