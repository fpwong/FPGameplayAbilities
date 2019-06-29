// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "FPGAAssetManager.generated.h"

class UFPGABaseItem;

/**
 * Game implementation of asset manager, overrides functionality and stores game-specific types
 * It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic
 * This is used by setting AssetManagerClassName in DefaultEngine.ini
 */
UCLASS()
	class FPGAMEPLAYABILITIES_API UFPGAAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UFPGAAssetManager()
	{
	}

	/** Static types for items */
	static const FPrimaryAssetType PotionItemType;
	static const FPrimaryAssetType SkillItemType;
	static const FPrimaryAssetType EquipmentItemType;

	/** Returns the current AssetManager object */
	static UFPGAAssetManager& Get();

	/**
	 * Synchronously loads an FPGAItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	UFPGABaseItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};
