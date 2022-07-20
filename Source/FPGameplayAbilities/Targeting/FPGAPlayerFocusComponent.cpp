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
		if (AActor* NewHovered = HitResult.GetActor())
		{
			if (TargetFilter.DoesFilterPass(nullptr, NewHovered))
			{
				AActor* OldHovered = HoveredActor.Get();
				if (OldHovered == NewHovered)
				{
					return;
				}

				if (OldHovered && OldHovered->Implements<UFPGAPlayerFocusInterface>())
				{
					IFPGAPlayerFocusInterface::Execute_OnEndHovered(OldHovered);
				}

				if (NewHovered && NewHovered->Implements<UFPGAPlayerFocusInterface>())
				{
					IFPGAPlayerFocusInterface::Execute_OnBeginHovered(NewHovered);
				}

				HoveredActor = NewHovered;
				OnHoveredActorChanged.Broadcast(OldHovered, NewHovered);
				return;
			}
		}
	}

	if (HoveredActor.IsValid())
	{
		AActor* OldHovered = HoveredActor.Get();
		if (OldHovered && OldHovered->Implements<UFPGAPlayerFocusInterface>())
		{
			IFPGAPlayerFocusInterface::Execute_OnEndHovered(OldHovered);
		}

		OnHoveredActorChanged.Broadcast(HoveredActor.Get(), nullptr);
		HoveredActor = nullptr;
	}
}

void UFPGAPlayerFocusComponent::SetFocus(AActor* NewFocus)
{
	AActor* OldFocused = FocusedActor.Get();

	if (OldFocused == NewFocus)
	{
		return;
	}

	if (NewFocus && !IsValidActor(NewFocus))
	{
		return;
	}

	if (OldFocused && OldFocused->Implements<UFPGAPlayerFocusInterface>())
	{
		IFPGAPlayerFocusInterface::Execute_OnEndFocus(OldFocused);
	}

	if (NewFocus && NewFocus->Implements<UFPGAPlayerFocusInterface>())
	{
		IFPGAPlayerFocusInterface::Execute_OnBeginFocus(NewFocus);
	}

	FocusedActor = NewFocus;
	OnFocusedActorChanged.Broadcast(OldFocused, NewFocus);
}

void UFPGAPlayerFocusComponent::FocusHoveredActor()
{
	SetFocus(GetHoveredActor());
}

bool UFPGAPlayerFocusComponent::IsValidActor(AActor* Actor)
{
	return TargetFilter.DoesFilterPass(nullptr, Actor);
}

