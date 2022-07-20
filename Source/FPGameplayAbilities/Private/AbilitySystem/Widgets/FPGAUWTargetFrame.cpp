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
		TargetActor = InTargetActor;

		SetVisibility(TargetActor ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
		check(HealthBar);
		HealthBar->BindAttributes(AbilitySystem);

		check(StatusBar);
		StatusBar->SetActor(InTargetActor);
	}
}