// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGAPlayerState.h"
#include "AbilitySystem/FPGAAttributeSet.h"
#include "AbilitySystem/FPGAAbilitySystemComponent.h"

AFPGAPlayerState::AFPGAPlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystem = CreateDefaultSubobject<UFPGAAbilitySystemComponent>(TEXT("AbilitySystem"));
	check(AbilitySystem != nullptr);
}

void AFPGAPlayerState::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySystem != nullptr);

	// Initialize actor info
	AbilitySystem->InitAbilityActorInfo(this, this);

	// Initialize the attribute set
	if (AttributeSetClass != nullptr)
	{
		AttributeSet = AbilitySystem->InitStats(AttributeSetClass, AttributeSetDataTable);

		if (HasAuthority())
		{
			for (int i = 0; i < InitialAbilities.Num(); i++)
			{
				TSubclassOf<UGameplayAbility> Ability = InitialAbilities[i];
				AbilitySystem->GiveAbility(FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, i));
			}

			for (TSubclassOf<UGameplayEffect> Effect : InitialEffects)
			{
				AbilitySystem->BP_ApplyGameplayEffectToSelf(Effect, 0.f, FGameplayEffectContextHandle());
			}
		}
	}
}

UAbilitySystemComponent* AFPGAPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}
