// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPUWAbilityFailedNotification.h"

#include "AbilitySystemComponent.h"
#include "CommonTextBlock.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

void UFPUWAbilityFailedNotification::BindToAbilitySystem(UAbilitySystemComponent* AbilitySystem)
{
	if (OldBoundAbilitySystem.IsValid())
	{
		AbilitySystem->AbilityFailedCallbacks.RemoveAll(this);
	}

	if (AbilitySystem)
	{
		AbilitySystem->AbilityFailedCallbacks.AddUObject(this, &ThisClass::HandleAbilityFailed);
	}

	OldBoundAbilitySystem = AbilitySystem;
}

void UFPUWAbilityFailedNotification::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& Tags)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *Tags.ToString());
	const FName FailureTag = UFPGAGameplayAbilitiesLibrary::GetSimpleTagName(Tags.First());
	Label->SetText(FText::FromName(FailureTag));

	BP_HandleAbilityFailed(Ability, Tags);
}
