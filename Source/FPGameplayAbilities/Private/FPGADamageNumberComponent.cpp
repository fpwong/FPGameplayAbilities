// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGADamageNumberComponent.h"

#include "AbilitySystem/Widgets/FPGAUWDamageNumber.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"

UFPGADamageNumberComponent::UFPGADamageNumberComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UFPGADamageNumberComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFPGADamageNumberComponent::Multicast_Damage_Implementation(AActor* DamagedActor, float DamageAmount, const FGameplayTagContainer& DamageTags)
{
	if (!DamagedActor || !DamageNumberClass)
	{
		return;
	}

	FDamageNumberInfo DamageNumberInfo;
	DamageNumberInfo.DamagedActor = DamagedActor;
	DamageNumberInfo.DamageAmount = DamageAmount;
	DamageNumberInfo.DamageTags = DamageTags;

	ActorDamageInfo.PendingDamageNumbers.Enqueue(DamageNumberInfo);

	TryPlayDamageNumber();

	// FActorDamageNumberInfo& NumberInfo = PendingDamageNumbers.FindOrAdd(DamagedActor);

	// const float Height = DamagedActor->GetSimpleCollisionHalfHeight();
	// FTransform Offset;
	// Offset.SetLocation(FVector(0, 0, Height));
	//
	// // spawn a new widget component
	// if (UWorld* World = GetWorld())
	// {
	// 	if (UFPGAUWDamageNumber* DamageNumber = CreateWidget<UFPGAUWDamageNumber>(World, DamageNumberClass))
	// 	{
	// 		UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(DamagedActor->AddComponentByClass(UWidgetComponent::StaticClass(), false, Offset, true));
	// 		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// 		WidgetComponent->SetWidget(DamageNumber);
	// 		WidgetComponent->SetDrawAtDesiredSize(true);
	//
	// 		DamageNumber->InitDamageNumber(WidgetComponent, DamageAmount, DamageTags);
	//
	// 		DamagedActor->FinishAddComponent(WidgetComponent, false, Offset);
	// 	}
	// }
}

void UFPGADamageNumberComponent::TryPlayDamageNumber()
{
	if (GetWorld()->TimeSince(LastPlayed) >= Delay)
	{
		PlayNextDamageNumber();
	}
}

void UFPGADamageNumberComponent::PlayNextDamageNumber()
{
	bWaitingForDamageNumber = !ActorDamageInfo.PendingDamageNumbers.IsEmpty();

	if (!ActorDamageInfo.PendingDamageNumbers.IsEmpty())
	{
		LastPlayed = GetWorld()->TimeSeconds;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UFPGADamageNumberComponent::PlayNextDamageNumber, Delay);

		FDamageNumberInfo DamageInfo;
		ActorDamageInfo.PendingDamageNumbers.Dequeue(DamageInfo);
		if (DamageInfo.DamagedActor.IsValid())
		{
			AActor* DamagedActor = DamageInfo.DamagedActor.Get();

			const float Height = DamagedActor->GetSimpleCollisionHalfHeight();
			FTransform Offset;
			Offset.SetLocation(FVector(0, 0, Height * 3));

			// spawn a new widget component
			if (UWorld* World = GetWorld())
			{
				if (UFPGAUWDamageNumber* DamageNumber = CreateWidget<UFPGAUWDamageNumber>(World, DamageNumberClass))
				{
					UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(DamagedActor->AddComponentByClass(UWidgetComponent::StaticClass(), true, Offset, true));
					WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
					WidgetComponent->SetWidget(DamageNumber);
					WidgetComponent->SetDrawAtDesiredSize(true);

					DamageNumber->InitDamageNumber(WidgetComponent, DamageInfo.DamageAmount, DamageInfo.DamageTags);

					DamagedActor->FinishAddComponent(WidgetComponent, false, Offset);

					if (auto Character = Cast<ACharacter>(DamagedActor))
					{
						WidgetComponent->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
					}

					// WidgetComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

					
				}
			}
		}
	}
}
