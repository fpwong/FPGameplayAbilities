// // Fill out your copyright notice in the Description page of Project Settings.
//
// #include "FPGAAssetManager.h"
// #include "Engine/Engine.h"
// // #include "FPGABaseItem.h"
//
// const FPrimaryAssetType	UFPGAAssetManager::PotionItemType = TEXT("Potion");
// const FPrimaryAssetType	UFPGAAssetManager::SkillItemType = TEXT("Skill");
// const FPrimaryAssetType	UFPGAAssetManager::EquipmentItemType = TEXT("Equipment");
//
// UFPGAAssetManager& UFPGAAssetManager::Get()
// {
// 	UFPGAAssetManager* This = Cast<UFPGAAssetManager>(GEngine->AssetManager);
//
// 	if (This)
// 	{
// 		return *This;
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be FPGAAssetManager!"));
// 		return *NewObject<UFPGAAssetManager>(); // never calls this
// 	}
// }
//
// UFPGABaseItem* UFPGAAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
// {	
// 	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);
//
// 	// This does a synchronous load and may hitch
// 	UFPGABaseItem* LoadedItem = Cast<UFPGABaseItem>(ItemPath.TryLoad());
//
// 	if (bLogWarning && LoadedItem == nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
// 	}
//
// 	return LoadedItem;
// }