// Fill out your copyright notice in the Description page of Project Settings.


#include "FPInteractionComponent.h"

#include "Kismet/KismetSystemLibrary.h"

bool UFPInteractionComponent::TryInteractWithFocus()
{
	if (FocusedActor.IsValid())
	{
		if (FocusedActor.IsValid() && FocusedActor->Implements<UFPInteractInterface>())
		{
			IFPInteractInterface::Execute_OnInteract(FocusedActor.Get(), GetOwner());
		}

		return true;
	}

	return false;
}

UFPInteractionComponent::UFPInteractionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OnBeginOverlap.AddDynamic(this, &ThisClass::BeginOverlap);
	OnEndOverlap.AddDynamic(this, &ThisClass::EndOverlap);
}

void UFPInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* NewFocus = FocusedActor.Get();
	float Closest = GetDistToActor(FocusedActor.Get());

	for (AActor* Actor : GetOverlappingActors())
	{
		const float Dist = GetDistToActor(Actor);
		if (Dist < Closest)
		{
			Closest = Dist;
			NewFocus = Actor;
		}
	}

	ChangeFocusedActor(FocusedActor.Get(), NewFocus);
}

void UFPInteractionComponent::BeginOverlap(AActor* Actor)
{
	if (Actor && !FocusedActor.IsValid())
	{
		ChangeFocusedActor(FocusedActor.Get(), Actor);
	}
}

void UFPInteractionComponent::EndOverlap(AActor* Actor)
{
	if (Actor && (Actor == FocusedActor))
	{
		ChangeFocusedActor(FocusedActor.Get(), nullptr);
	}
}

void UFPInteractionComponent::ChangeFocusedActor(AActor* Old, AActor* New)
{
	if (FocusedActor != New)
	{
		FocusedActor = New;
		OnFocusedActorChanged.Broadcast(Old, New);

		if (Old && Old->Implements<UFPInteractInterface>())
		{
			IFPInteractInterface::Execute_OnLoseFocus(Old);
		}

		if (New && New->Implements<UFPInteractInterface>())
		{
			IFPInteractInterface::Execute_OnGainFocus(New);
		}
	}
}

float UFPInteractionComponent::GetDistToActor(AActor* Actor)
{
	if (!Actor)
	{
		return FLT_MAX;
	}

	return FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
}
