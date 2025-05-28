

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "FPGAAsyncTask_EffectAddedOrRemoved.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayEffectAddedOrRemoved, FGameplayTag, EffectGameplayTag, FActiveGameplayEffectHandle, Handle);

/**
 * Blueprint node to automatically register a listener for changes to a GameplayEffect's stack count based on an Asset or Granted tag on the Effect.
 * Useful to use in UI.
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class FPGAMEPLAYABILITIES_API UFPGAAsyncTask_EffectAddedOrRemoved : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintAssignable)
	FOnGameplayEffectAddedOrRemoved OnGameplayEffectAdded;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayEffectAddedOrRemoved OnGameplayEffectRemoved;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UFPGAAsyncTask_EffectAddedOrRemoved* ListenForGameplayEffectAddedOrRemoved(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTag EffectGameplayTag, bool bInCheckExactTag);

	// You must call this function manually when you want the AsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag EffectGameplayTag;

	bool bCheckExactTag;

	virtual void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	virtual void OnRemoveGameplayEffectCallback(const FActiveGameplayEffect& EffectRemoved);
};
