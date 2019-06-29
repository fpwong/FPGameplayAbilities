// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAAttributeSet.h"
#include "Math/UnrealMathUtility.h"

#include "Net/UnrealNetwork.h"
#include "GameplayTagsManager.h"
#include "GameplayEffectExtension.h"
#include "FPGACharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FPGAAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

UFPGAAttributeSet::UFPGAAttributeSet()
{
	// Misc
	MaxHealth = Health = 100.f;
	MovementSpeed = 400.f;
}


bool UFPGAAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData &Data)
{
	return UAttributeSet::PreGameplayEffectExecute(Data);
}

void UFPGAAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	UAttributeSet::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UFPGAAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{

}

void UFPGAAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPGAAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPGAAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UFPGAAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	//----------------
	//	This doesn't get called by effects which have infinite duration
	//----------------

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();	

	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Get the Source actor
	AActor* SourceActor = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive)
		{
			// you can only lose as much health as you have
			const float OldHealth = GetHealth() - Data.EvaluatedData.Magnitude;

			float HealthChange = Data.EvaluatedData.Magnitude;

			if (HealthChange < 0) // lost health
			{
				HandleHealthLost(SourceActor, -1 * HealthChange, OldHealth);
			}
			else if (HealthChange > 0) // gained health
			{
				if (GetHealth() > GetMaxHealth()) // over heal!
				{
					const float Overheal = GetHealth() - GetMaxHealth();
					HealthChange -= Overheal;
					SetHealth(GetMaxHealth());
				}

				FGameplayEventData DataHealthGained;
				DataHealthGained.EventMagnitude = HealthChange;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), FGameplayTag::RequestGameplayTag(FName("Event.Health.Gained"), false), DataHealthGained);
			}
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0)
		{
			HandleHealthLost(SourceActor, LocalDamageDone, GetHealth());
		}
	}
}

void UFPGAAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttribute(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UFPGAAttributeSet::HandleHealthLost(AActor* SourceActor, float HealthLost, const float HealthBeforeLost)
{
	if (HealthLost >= HealthBeforeLost) // we died
	{
		if (HealthLost > HealthBeforeLost)
		{
			float Overkill = HealthLost - HealthBeforeLost;
			HealthLost -= Overkill;
			SetHealth(0);
		}
	}
	
	SetHealth(HealthBeforeLost - HealthLost);

	if (HealthLost > 0)
	{
		FGameplayEventData DataHealthLost;
		DataHealthLost.OptionalObject = SourceActor;
		DataHealthLost.EventMagnitude = HealthLost;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), FGameplayTag::RequestGameplayTag(FName("Event.Health.Lost"), false), DataHealthLost);

		if (SourceActor != nullptr)
		{
			FGameplayEventData DataDamageDealt;
			DataDamageDealt.OptionalObject = GetOwningActor();
			DataDamageDealt.EventMagnitude = HealthLost;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceActor, FGameplayTag::RequestGameplayTag(FName("Event.Damage.Dealt"), false), DataDamageDealt);
		}
	}
}
