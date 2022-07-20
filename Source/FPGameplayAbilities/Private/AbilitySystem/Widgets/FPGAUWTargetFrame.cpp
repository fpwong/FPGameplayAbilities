// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWTargetFrame.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UFPGAUWTargetFrame::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
}

void UFPGAUWTargetFrame::SetActor_Implementation(AActor* InTargetActor)
{
	if (InTargetActor != TargetActor)
	{
		SetVisibility(InTargetActor ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InTargetActor);
		check(HealthBar);
		HealthBar->BindAttributes(AbilitySystem);

		check(StatusBar);
		StatusBar->SetActor(InTargetActor);

		TargetActor = InTargetActor;
	}
}

void UFPGAUWTargetFrame::BindToPlayerFocus_Implementation(UFPGAPlayerFocusComponent* InPlayerFocusComponent)
{
	if (!InPlayerFocusComponent)
	{
		return;
	}

	InPlayerFocusComponent->OnHoveredActorChanged.AddUniqueDynamic(this, &ThisClass::HandleHoveredActorChanged);
	InPlayerFocusComponent->OnFocusedActorChanged.AddUniqueDynamic(this, &ThisClass::HandleFocusedActorChanged);

	PlayerFocusComponent = InPlayerFocusComponent;
}

void UFPGAUWTargetFrame::HandleHoveredActorChanged(AActor* OldActor, AActor* NewActor)
{
	// the focused actor should override the hovered actor
	if (PlayerFocusComponent->GetFocusedActor())
	{
		return;
	}

	SetActor(NewActor);
}

void UFPGAUWTargetFrame::HandleFocusedActorChanged(AActor* OldActor, AActor* NewActor)
{
	if (!NewActor)
	{
		// fallback to the hovered actor
		SetActor(PlayerFocusComponent->GetHoveredActor());
	}
	else
	{
		SetActor(NewActor);
	}
}
