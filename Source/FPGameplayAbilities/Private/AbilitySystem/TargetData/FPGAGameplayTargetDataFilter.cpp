// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/TargetData/FPGAGameplayTargetDataFilter.h"
#include "GameplayTagAssetInterface.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

FFPGAGameplayTargetDataFilter::FFPGAGameplayTargetDataFilter()
{
	OwnerTagQuery = FGameplayTagQuery::EmptyQuery;
}

bool FFPGAGameplayTargetDataFilter::FilterPassesForActor(const AActor* ActorToBeFiltered) const
{
	if (ActorToBeFiltered == nullptr)
		return false;

	if (CustomMatchDelegate.IsBound())
	{
		if (CustomMatchDelegate.Execute(ActorToBeFiltered) == false)
		{
			return false;
		}
	}

	if (CustomMatchDelegate_BP.IsBound())
	{
		bool bDelegateMatches = false;
		CustomMatchDelegate_BP.Execute(ActorToBeFiltered, bDelegateMatches);
		if (bDelegateMatches == false)
		{
			return false;
		}
	}

	// IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(SelfActor);
	// if (TeamAgent != nullptr)
	// {
	// 	if (!FAISenseAffiliationFilter::ShouldSenseTeam(TeamAgent, *ActorToBeFiltered, Affiliation.GetAsFlags()))
	// 		return false;
	// }

	if (!OwnerTagQuery.IsEmpty())
	{
		if (const IGameplayTagAssetInterface * TagAssetInterface = Cast<IGameplayTagAssetInterface>(ActorToBeFiltered))
		{
			FGameplayTagContainer TagContainer = FGameplayTagContainer::EmptyContainer;
			TagAssetInterface->GetOwnedGameplayTags(TagContainer);
			if (!OwnerTagQuery.Matches(TagContainer))
				return false;
		}
	}

	if (!RequiredEffectQuery.IsEmpty() || RequiredEffectQuery.CustomMatchDelegate_BP.IsBound() || RequiredEffectQuery.CustomMatchDelegate.IsBound())
	{
		if (const IAbilitySystemInterface * AbilitySystemInterface = Cast<IAbilitySystemInterface>(ActorToBeFiltered))
		{
			UAbilitySystemComponent* AbilitySystem = AbilitySystemInterface->GetAbilitySystemComponent();

			if (AbilitySystem->GetActiveEffects(RequiredEffectQuery).Num() == 0)
				return false;
		}
	}

	if (!BlockedEffectQuery.IsEmpty() || BlockedEffectQuery.CustomMatchDelegate_BP.IsBound() || BlockedEffectQuery.CustomMatchDelegate.IsBound())
	{
		if (const IAbilitySystemInterface * AbilitySystemInterface = Cast<IAbilitySystemInterface>(ActorToBeFiltered))
		{
			UAbilitySystemComponent* AbilitySystem = AbilitySystemInterface->GetAbilitySystemComponent();

			if (AbilitySystem->GetActiveEffects(BlockedEffectQuery).Num() > 0)
				return false;
		}
	}

	return true;
}
