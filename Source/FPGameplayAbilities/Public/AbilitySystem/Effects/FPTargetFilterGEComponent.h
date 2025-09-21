// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilter.h"
#include "UObject/Object.h"
#include "FPTargetFilterGEComponent.generated.h"

UCLASS(DisplayName="Target Filter")
class UFPTargetFilterGEComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFPTargetFilter TargetFilter;

	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
};
