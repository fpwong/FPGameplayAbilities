// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGADamageNumberComponent.h"

#include "AbilitySystem/Widgets/FPGAUWDamageNumber.h"
#include "Components/WidgetComponent.h"

UFPGADamageNumberComponent::UFPGADamageNumberComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UFPGADamageNumberComponent::Multicast_Damage_Implementation(AActor* DamagedActor, float DamageAmount, const FGameplayTagContainer& DamageTags)
{
	if (!DamagedActor || !DamageNumberClass)
	{
		return;
	}

	const float Height = DamagedActor->GetSimpleCollisionHalfHeight();
	FTransform Offset;
	Offset.SetLocation(FVector(0, 0, Height));

	// spawn a new widget component
	if (UWorld* World = GetWorld())
	{
		if (UFPGAUWDamageNumber* DamageNumber = CreateWidget<UFPGAUWDamageNumber>(World, DamageNumberClass))
		{
			UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(DamagedActor->AddComponentByClass(UWidgetComponent::StaticClass(), false, Offset, true));
			WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
			WidgetComponent->SetWidget(DamageNumber);
			WidgetComponent->SetDrawAtDesiredSize(true);

			DamageNumber->InitDamageNumber(WidgetComponent, DamageAmount, DamageTags);

			DamagedActor->FinishAddComponent(WidgetComponent, false, Offset);
		}
	}
}

