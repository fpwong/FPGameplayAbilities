// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAPlayerFocusComponent.h"

// Sets default values for this component's properties
UFPGAPlayerFocusComponent::UFPGAPlayerFocusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UFPGAPlayerFocusComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFPGAPlayerFocusComponent::UpdateStateFromHitResults(const TArray<FHitResult>& HitResults)
{
	for (const auto& HitResult : HitResults)
	{
		if (auto HitActor = HitResult.GetActor())
		{
			if (TargetFilter.DoesFilterPass(nullptr, HitActor))
			{
				if (HoveredActor.Get() == HitActor)
				{
					return;
				}

				if (HoveredActor.IsValid())
				{
					if (HoveredActor.Get()->Implements<UFPGAPlayerFocusInterface>())
					{
						IFPGAPlayerFocusInterface::Execute_OnEndFocus(HoveredActor.Get());
					}

					HoveredActor.Reset();
				}

				OnHoveredActorChanged.Broadcast(HoveredActor.Get(), HitActor);

				HoveredActor = HitActor;
				if (HitActor->Implements<UFPGAPlayerFocusInterface>())
				{
					IFPGAPlayerFocusInterface::Execute_OnBeginFocus(HitActor);
				}

				return;
			}
		}
	}

	if (HoveredActor.IsValid())
	{
		OnHoveredActorChanged.Broadcast(HoveredActor.Get(), nullptr);
		HoveredActor = nullptr;
	}
}

