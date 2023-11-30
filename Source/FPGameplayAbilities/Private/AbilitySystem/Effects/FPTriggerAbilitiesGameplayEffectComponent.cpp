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
		FGameplayAbilitySpecHandle Handle = UFPGAGameplayAbilitiesLibrary::GiveAbility(ASC, Ability);
		ActiveGE.GrantedAbilityHandles.Add(Handle);

		FGameplayEventData EventData;
		EventData.ContextHandle = ActiveGE.Spec.GetContext();

		if (UGameplayAbility* GrantedAbility = UFPGAGameplayAbilitiesLibrary::ActivateAbilityWithEvent(ASC, Handle, EventData))
		{
			ASC->SetRemoveAbilityOnEnd(GrantedAbility->GetCurrentAbilitySpecHandle());
		}
	}

	return true;
}
