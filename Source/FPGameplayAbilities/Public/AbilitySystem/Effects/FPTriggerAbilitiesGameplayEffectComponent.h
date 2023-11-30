// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectComponent.h"
#include "FPTriggerAbilitiesGameplayEffectComponent.generated.h"

/** Add additional Gameplay Effects that attempt to activate under certain conditions (or no conditions) */
UCLASS(CollapseCategories)
class FPGAMEPLAYABILITIES_API UFPTriggerAbilitiesGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& GEContainer, FActiveGameplayEffect& ActiveGE) const override;

	/** Abilities to Grant to the Target while this Gameplay Effect is active */
	UPROPERTY(EditDefaultsOnly, Category = OnApplication)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
};
