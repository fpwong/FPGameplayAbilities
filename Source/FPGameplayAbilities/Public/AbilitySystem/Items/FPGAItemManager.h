// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystem/Items/FPGABaseItem.h"
#include "FPGAItemManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChanged, FFPGAItemSlot, ItemSlot, UFPGABaseItem*, Item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, UFPGABaseItem*, Item, int32, NewCount);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
	class FPGAMEPLAYABILITIES_API UFPGAItemManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<class UGameplayAbility> EquipAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<class UGameplayAbility> UnequipAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<class UGameplayAbility> BuyItemAbility;

	/** Delegate called when an inventory slot has changed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnSlottedItemChanged OnSlottedItemChanged;

	/** Delegate called when an item has been added or removed has changed */
	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOnInventoryChanged OnInventoryChanged;

	/** Map of slot, from type/num to item, initialized from ItemSlotsPerType on RPGGameInstanceBase */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FPrimaryAssetId, int32> InventoryItems;

	/** Map of slot, from type/num to item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FFPGAItemSlot, UFPGABaseItem*> SlottedItems;

	// Sets default values for this component's properties
	UFPGAItemManager();

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void EquipItem(UFPGABaseItem* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipItem(UFPGABaseItem* Item);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void UnequipItem(UFPGABaseItem* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UnequipItem(UFPGABaseItem* Item);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void AddItem(UFPGABaseItem* Item, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetNumItems(UFPGABaseItem* Item);

	/** Adds a new inventory item, will add it to an empty slot if possible. 
		If the item supports count you can add more than one count. 
		It will also update the level when adding if required */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SetSlottedItem(FFPGAItemSlot ItemSlot, UFPGABaseItem* Item, bool bUpdateInventory = true);

	/** Returns item in slot, or null if empty */
	UFUNCTION(BlueprintPure, Category = Inventory)
	UFPGABaseItem* GetSlottedItem(FFPGAItemSlot ItemSlot) const;
};
