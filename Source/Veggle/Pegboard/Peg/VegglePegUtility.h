// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VegglePegUtility.generated.h"

USTRUCT(BlueprintType)
struct FVegglePegInstanceData : public FTableRowBase
{
	GENERATED_BODY()

	FVegglePegInstanceData() 
		: PegTransform(FTransform::Identity)
		, PegData(nullptr)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeggleBall")
	FTransform PegTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeggleBall")
	TObjectPtr<UVegglePegData> PegData;
};