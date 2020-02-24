// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "FPGAAsyncTask_TagChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTagChanged, const FGameplayTag, Tag, int32, NewCount);

/**
 * Blueprint node to automatically register a listener for changes (Begin and End) to an array of Tag tags.
 * Useful to use in UI.
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = FPGAAsyncTask))
class FPGAMEPLAYABILITIES_API UFPGAAsyncTask_TagChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnTagChanged OnTagChanged;

	// Listens for changes (Begin and End) to Tag GameplayEffects based on the Tag tag.
	// UseServerTag determines if the Sever's Tag is returned in addition to the local predicted Tag.
	// If using ServerTag, TimeRemaining and Duration will return -1 to signal local predicted Tag has begun.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UFPGAAsyncTask_TagChanged* ListenForTagChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag Tag, EGameplayTagEventType::Type EventType);
	
	// You must call this function manually when you want the FPGAAsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayTag Tag;

	EGameplayTagEventType::Type EventType;

	virtual void TagChanged(const FGameplayTag TagTag, int32 NewCount);
};
