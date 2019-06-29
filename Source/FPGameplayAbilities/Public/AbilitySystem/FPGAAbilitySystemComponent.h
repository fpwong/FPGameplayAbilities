// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "FPGAAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotifyEvent, FName, EventName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInputCancel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayEffectDelegate, FActiveGameplayEffectHandle, ActiveHandle);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FFPGAAttributeChangedDelegate, float, OldValue, float, NewValue);


UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey = FPredictionKey()) override;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Ability|Input")
	FOnInputCancel OnInputCancel;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Ability|GameplayEffect")
	FGameplayEffectDelegate OnGameplayEffectAppliedToSelf;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Ability|GameplayEffect")
	FGameplayEffectDelegate OnGameplayEffectRemoved;

	UFUNCTION(BlueprintCallable)
	virtual void LocalInputConfirm() override;

	UFUNCTION(BlueprintCallable)
	virtual void LocalInputCancel() override;

	FActiveGameplayEffectsContainer& GetActiveGameplayEffects() { return ActiveGameplayEffects; }

	UFUNCTION()
	void OnGEAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	UFUNCTION()
	void OnGERemoved(const FActiveGameplayEffect& ActiveGameplayEffect);

	UFUNCTION(BlueprintCallable)
	void BindToAttributeChanged(FGameplayAttribute Attribute, FFPGAAttributeChangedDelegate OnAttributeChanged);

	UFUNCTION(BlueprintCallable)
	void K2_RegisterGameplayTagEvent(FGameplayTag EventTag);

private:
	TMap<FGameplayAttribute, FFPGAAttributeChangedDelegate> BP_AttributeValueChangeDelegates;
	void ExecuteOnAttributeChanged(const FOnAttributeChangeData& ChangeData);
};