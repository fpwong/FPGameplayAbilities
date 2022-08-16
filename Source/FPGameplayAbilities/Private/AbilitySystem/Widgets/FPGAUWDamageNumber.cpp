// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWDamageNumber.h"

#include "CommonTextBlock.h"
#include "Components/WidgetComponent.h"

void UFPGAUWDamageNumber::InitDamageNumber(UWidgetComponent* InOwnerWidgetComponent, float Damage, const FGameplayTagContainer& DamageTags)
{
	OwnerWidgetComponent = InOwnerWidgetComponent;
	DamageNumberLabel->SetText(FText::FromString(FString::Printf(TEXT("%f"), Damage)));

	BP_InitDamageNumber(InOwnerWidgetComponent, Damage, DamageTags);
}

void UFPGAUWDamageNumber::EndDamageNumber()
{
	if (OwnerWidgetComponent)
	{
		OwnerWidgetComponent->DestroyComponent();
	}

	OnDamageNumberEnded.Broadcast(this);
}
