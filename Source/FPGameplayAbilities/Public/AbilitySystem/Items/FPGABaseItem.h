// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "UObject/PrimaryAssetId.h"
#include "Styling/SlateBrush.h"
#include "FPGABaseItem.generated.h"

/** Struct representing a slot for an item, shown in the UI */
USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGAItemSlot
{
	GENERATED_BODY()

	/** Constructor, -1 means an invalid slot */
	FFPGAItemSlot()
		: SlotNumber(-1)
	{}

	FFPGAItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
		: ItemType(InItemType)
		, SlotNumber(InSlotNumber)
	{}

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	/** Equality operators */
	bool operator==(const FFPGAItemSlot& Other) const
	{
		return ItemType == Other.ItemType && SlotNumber == Other.SlotNumber;
	}
	bool operator!=(const FFPGAItemSlot& Other) const
	{
		return !(*this == Other);
	}

	/** Implemented so it can be used in Maps/Sets */
	friend inline uint32 GetTypeHash(const FFPGAItemSlot& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	/** Returns true if slot is valid */
	bool IsValid() const
	{
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};


/** Base class for all items, do not blueprint directly */
UCLASS(Abstract, BlueprintType)
class FPGAMEPLAYABILITIES_API UFPGABaseItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Constructor */
	UFPGABaseItem()
		: Price(0)
		, MaxCount(1)
		, MaxLevel(1)
	{}

	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FSlateBrush ItemIcon;

	/** Price in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 Price;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxCount;

	/** Maximum level this item can be, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxLevel;

	/** Ability to grant if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	/** Effects to apply if this item is slotted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effects)
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;

	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	void ApplyItem(class UAbilitySystemComponent* AbilitySystem);
	
	void RemoveItem(class UAbilitySystemComponent* AbilitySystem);

private:
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};


