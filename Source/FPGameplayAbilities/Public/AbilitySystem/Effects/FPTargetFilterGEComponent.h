// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilter.h"
#include "UObject/Object.h"
#include "FPTargetFilterGEComponent.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayEffectRemovalInfo;
class UAbilitySystemComponent;

UCLASS(DisplayName="Target Filter")
class UFPTargetFilterGEComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFPTargetFilter TargetFilter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bInhibits = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bBlocksApplication = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRemoveOnFail = false;

	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;

	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;

	void OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& GERemovalInfo, UAbilitySystemComponent* ASC, TSharedPtr<FFPTargetFilterObserver> Observer) const;

	void OnResultChanged(AActor* Actor, bool bFilterPassed, FActiveGameplayEffectHandle ActiveGEHandle) const;
};
