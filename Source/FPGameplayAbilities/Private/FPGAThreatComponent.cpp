// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAThreatComponent.h"

// Sets default values for this component's properties
UFPGAThreatComponent::UFPGAThreatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UFPGAThreatComponent::AddThreat(UFPGAThreatComponent* Target, float Threat)
{
	ThreatTable.FindOrAdd(Target) += Threat;

	UpdateAggroTarget(Target);
}

float UFPGAThreatComponent::GetThreat(UFPGAThreatComponent* Target)
{
	if (float* ThreatPtr = ThreatTable.Find(Target))
	{
		return *ThreatPtr; 
	}

	return -1.0f;
}

UFPGAThreatComponent* UFPGAThreatComponent::GetAggroTarget()
{
	return AggroTarget.IsValid() ? AggroTarget.Get() : nullptr;
}

void UFPGAThreatComponent::UpdateAggroTarget(UFPGAThreatComponent* NewTarget)
{
	if (AggroTarget.IsValid())
	{
		const float HighestThreat = GetThreat(AggroTarget.Get());

		// we need to compare against all other threat values
		if (AggroTarget == NewTarget)
		{
			UFPGAThreatComponent* NewHighestThreat = GetHighestThreat();
			if (AggroTarget != NewHighestThreat)
			{
				SetAggroTarget(NewHighestThreat);
			}
		}
		else // we are not the highest threat, we only care about checking against the aggro target
		{
			if (float NewThreat = GetThreat(NewTarget))
			{
				if (NewThreat > HighestThreat)
				{
					SetAggroTarget(NewTarget);
				}
			}
		}
	}
	else
	{
		SetAggroTarget(NewTarget);
	}
}

void UFPGAThreatComponent::SetAggroTarget(UFPGAThreatComponent* NewTarget)
{
	if (AggroTarget == NewTarget)
	{
		return;
	}

	UFPGAThreatComponent* OldTarget = nullptr;
	if (AggroTarget.IsValid())
	{
		OldTarget = AggroTarget.Get();
	}

	AggroTarget = NewTarget;

	OnThreatChanged.Broadcast(OldTarget, NewTarget);
}

UFPGAThreatComponent* UFPGAThreatComponent::GetHighestThreat()
{
	ThreatTable.ValueSort([](const float A, const float B)
	{
		return A > B; // highest threat first
	});

	for (const auto& Elem : ThreatTable)
	{
		if (Elem.Key.IsValid())
		{
			return Elem.Key.Get();
		}

		break;
	}

	return nullptr;
}

// Called when the game starts
void UFPGAThreatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

