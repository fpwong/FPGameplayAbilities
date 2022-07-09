// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGATargetFilter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

bool FFPGATargetFilter::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
{
	if (!SourceActor || !TargetActor)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!AbilitySystem)
	{
		return false;
	}

	if (!TargetTagQuery.IsEmpty())
	{
		FGameplayTagContainer TargetTags;

		// TODO: Add affiliation
		AbilitySystem->GetOwnedGameplayTags(TargetTags);
		if (!TargetTagQuery.Matches(TargetTags))
		{
			return false;
		}
	}

	if (!RequiredEffectQuery.IsEmpty() || RequiredEffectQuery.CustomMatchDelegate_BP.IsBound() || RequiredEffectQuery.CustomMatchDelegate.IsBound())
	{
		if (AbilitySystem->GetActiveEffects(RequiredEffectQuery).Num() == 0)
			return false;
	}

	if (!BlockedEffectQuery.IsEmpty() || BlockedEffectQuery.CustomMatchDelegate_BP.IsBound() || BlockedEffectQuery.CustomMatchDelegate.IsBound())
	{
		if (AbilitySystem->GetActiveEffects(BlockedEffectQuery).Num() > 0)
			return false;
	}

	return true;
}
