// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Effects/FPTriggerAbilitiesGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "AbilitySystem/FPGAGlobalTags.h"
#include "Misc/DataValidation.h"

bool UFPTriggerAbilitiesGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	// We don't allow prediction of expiration (on removed) effects
	if (!ActiveGEContainer.IsNetAuthority())
	{
		return true;
	}

	UAbilitySystemComponent* ASC = ActiveGEContainer.Owner;
	for (const TSubclassOf<UGameplayAbility>& Ability : GrantedAbilities)
	{
		if (ASC->FindAbilitySpecFromClass(Ability))
		{
			continue;
		}

		FGameplayEventData EventData;
		EventData.ContextHandle = ActiveGE.Spec.GetContext();

		FGameplayAbilitySpecHandle Handle = UFPGAGameplayAbilitiesLibrary::GiveAbilityAndActivateOnce(ASC, Ability, EventData);
		if (Handle.IsValid())
		{
			ActiveGE.GrantedAbilityHandles.Add(Handle);

			ASC->SetRemoveAbilityOnEnd(Handle);
		}
	}

	return true;
}
