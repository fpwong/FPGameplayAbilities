// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Items/FPGABaseItem.h"
#include "FPGAAssetManager.h"
#include "FPGASkillItemAsset.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGASkillItemAsset : public UFPGABaseItem
{
	GENERATED_BODY()
	
	UFPGASkillItemAsset()
	{
		ItemType = UFPGAAssetManager::SkillItemType;
	}
};
