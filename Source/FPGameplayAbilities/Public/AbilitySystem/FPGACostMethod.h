// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "UObject/Object.h"
#include "FPGACostMethod.generated.h"

UCLASS(DefaultToInstanced, EditInlineNew, CollapseCategories, Abstract)
class FPGAMEPLAYABILITIES_API UFPGACostMethod : public UObject
{
	GENERATED_BODY()

public:
	virtual void ApplyCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const {}
	virtual bool CheckCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const { return false; }
};

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API UFPGACostMethod_Attribute : public UFPGACostMethod 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayModifierInfo> AbilityCosts;

	virtual UGameplayEffect* GetCostGameplayEffect() const;

	virtual void ApplyCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool CheckCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
};

