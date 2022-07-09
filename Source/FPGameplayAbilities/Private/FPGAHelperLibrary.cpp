// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAHelperLibrary.h"

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
