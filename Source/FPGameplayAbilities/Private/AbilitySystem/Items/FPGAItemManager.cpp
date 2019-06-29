// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Items/FPGAItemManager.h"
#include "AbilitySystemComponent.h"
#include "FPGAAssetManager.h"

// Sets default values for this component's properties
UFPGAItemManager::UFPGAItemManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UFPGAItemManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UFPGAItemManager::EquipItem(UFPGABaseItem* Item)
{
}

void UFPGAItemManager::Server_EquipItem_Implementation(UFPGABaseItem* Item)
{
	UAbilitySystemComponent* AbilitySystem = Cast<UAbilitySystemComponent>(GetOwner()->FindComponentByClass(UAbilitySystemComponent::StaticClass()));
	if (AbilitySystem && Item)
	{
		Item->ApplyItem(AbilitySystem);
	}
}

bool UFPGAItemManager::Server_EquipItem_Validate(UFPGABaseItem* Item)
{
	return true;
}

void UFPGAItemManager::UnequipItem(UFPGABaseItem* Item)
{
}

void UFPGAItemManager::Server_UnequipItem_Implementation(UFPGABaseItem* Item)
{
	UAbilitySystemComponent* AbilitySystem = Cast<UAbilitySystemComponent>(GetOwner()->FindComponentByClass(UAbilitySystemComponent::StaticClass()));
	if (AbilitySystem && Item)
	{
		Item->RemoveItem(AbilitySystem);
	}
}

bool UFPGAItemManager::Server_UnequipItem_Validate(UFPGABaseItem* Item)
{
	return true;
}

void UFPGAItemManager::AddItem(UFPGABaseItem* Item, int32 Count)
{
	if (!Item)
		return;
	InventoryItems.FindOrAdd(Item->GetPrimaryAssetId()) += Count;
	OnInventoryChanged.Broadcast(Item, Count);
}

int32 UFPGAItemManager::GetNumItems(UFPGABaseItem* Item)
{
	if (!Item)
		return 0;

	const FPrimaryAssetId AssetId = Item->GetPrimaryAssetId();
	return InventoryItems.Contains(AssetId) ? InventoryItems[AssetId] : 0;
}

bool UFPGAItemManager::SetSlottedItem(FFPGAItemSlot ItemSlot, UFPGABaseItem* Item, bool bUpdateInventory)
{
	if (SlottedItems.Contains(ItemSlot))
	{
		if (UFPGABaseItem* OldItem = SlottedItems[ItemSlot])
		{
			Server_UnequipItem(OldItem);

			if (bUpdateInventory && OldItem->ItemType == UFPGAAssetManager::EquipmentItemType)
			{
				AddItem(OldItem, 1);
			}
		}
	}

	if (Item != nullptr)
	{
		Server_EquipItem(Item);

		if (bUpdateInventory && Item->ItemType == UFPGAAssetManager::EquipmentItemType)
		{
			AddItem(Item, -1);
		}
	}

	SlottedItems.Add(ItemSlot, Item);
	OnSlottedItemChanged.Broadcast(ItemSlot, Item);
	return true;
}

UFPGABaseItem* UFPGAItemManager::GetSlottedItem(FFPGAItemSlot ItemSlot) const
{
	UFPGABaseItem* const* FoundItem = SlottedItems.Find(ItemSlot);
	if (FoundItem)
	{
		return *FoundItem;
	}

	return nullptr;
}
