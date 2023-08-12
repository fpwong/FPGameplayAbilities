// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAHelperLibrary.h"

#include "GameplayTagContainer.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilterTask_GameplayTag.h"
#include "Perception/AISense.h"

bool UFPGAHelperLibrary::CheckAffiliation(FAISenseAffiliationFilter Affiliation, AActor* ActorA, AActor* ActorB)
{
	if (ActorA != nullptr && ActorB != nullptr)
	{
		if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(ActorA))
		{
			return FAISenseAffiliationFilter::ShouldSenseTeam(TeamAgent, *ActorB, Affiliation.GetAsFlags());
		}
	}

	return false;
}

TMap<FGameplayTag, int32> UFPGAHelperLibrary::GetTagCountMapping(const FGameplayTagCountContainer& TagCountContainer)
{
	TMap<FGameplayTag, int32> OutMap;

	const FGameplayTagContainer& ExplicitTags = TagCountContainer.GetExplicitGameplayTags();
	
	for (auto TagIt = ExplicitTags.CreateConstIterator(); TagIt; ++TagIt)
	{
		const FGameplayTag& Tag = *TagIt;
		OutMap.Add(Tag, TagCountContainer.GetTagCount(Tag));
	}

	return OutMap;
}
