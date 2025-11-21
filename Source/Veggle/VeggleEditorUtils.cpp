// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.


#include "VeggleEditorUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VeggleEditorUtils)

bool UVeggleEditorUtils::ClearDataTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		return false;
	}

	DataTable->EmptyTable();
	return true;
}
