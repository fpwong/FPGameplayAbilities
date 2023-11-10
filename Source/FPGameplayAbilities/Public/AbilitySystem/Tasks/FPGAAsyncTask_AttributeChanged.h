// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FPGAAsyncTask_AttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

/**
 * Blueprint node to automatically register a listener for all attribute changes in an AbilitySystemComponent.
 * Useful to use in UI.
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class FPGAMEPLAYABILITIES_API UFPGAAsyncTask_AttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;

	// Listens for an attribute changing.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UFPGAAsyncTask_AttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UFPGAAsyncTask_AttributeChanged* ListenForAttributeChangeWithTags(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, FGameplayTagContainer Tags);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	UFUNCTION(BlueprintCallable)
	float CalculateAttributeValue();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayAttribute AttributeToListenFor;
	FGameplayTagContainer EvalTags;

	FGameplayEffectAttributeCaptureSpec CaptureSpec;
	FAggregatorEvaluateParameters EvalParams;

	float OldCalculatedValue;

	void Init(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute);
	void BindAttributeChanged();
	void BindAttributeChangedWithTags(const FGameplayTagContainer& Tags);

	void AttributeChanged(const FOnAttributeChangeData& Data);
	void AttributeChangedWithTags(const FOnAttributeChangeData& Data);

	UFUNCTION()
	void OnActorDestroyed(AActor* DestroyedActor);
};
