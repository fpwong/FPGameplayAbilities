// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Items/FPGABaseItem.h"
#include "FPGAAssetManager.h"
#include "FPGAEquipmentAsset.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAEquipmentAsset : public UFPGABaseItem
{
	GENERATED_BODY()
	
	UFPGAEquipmentAsset()
	{
		ItemType = UFPGAAssetManager::EquipmentItemType;
	}
};
