// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWTargetFrame.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FPGameplayAbilities/Targeting/FPGATargetingSubsystem.h"

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

void UFPGAUWTargetFrame::BindToTargetingSubsystem_Implementation()
{
	auto TargetingSystem = UFPGATargetingSubsystem::Get(GetWorld()->GetFirstLocalPlayerFromController());
	check(TargetingSystem);

	TargetingSystem->OnHoveredActorChanged.AddUniqueDynamic(this, &ThisClass::HandleHoveredActorChanged);
	TargetingSystem->OnFocusedActorChanged.AddUniqueDynamic(this, &ThisClass::HandleFocusedActorChanged);
}

void UFPGAUWTargetFrame::HandleHoveredActorChanged(AActor* OldActor, AActor* NewActor)
{
	// the focused actor should override the hovered actor
	// if (PlayerFocusComponent->GetFocusedActor())
	HoveredActor = NewActor;

	if (FocusedActor.IsValid())
	{
		return;
	}

	SetActor(NewActor);
}

void UFPGAUWTargetFrame::HandleFocusedActorChanged(AActor* OldActor, AActor* NewActor)
{
	FocusedActor = NewActor;

	if (!NewActor)
	{
		// fallback to the hovered actor
		SetActor(HoveredActor.Get());
	}
	else
	{
		SetActor(NewActor);
	}
}
