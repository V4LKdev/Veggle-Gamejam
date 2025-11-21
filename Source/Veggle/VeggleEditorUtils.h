// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VeggleEditorUtils.generated.h"

/**
 * 
 */
UCLASS()
class VEGGLE_API UVeggleEditorUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PegData")
	static bool ClearDataTable(UDataTable* DataTable);
};
