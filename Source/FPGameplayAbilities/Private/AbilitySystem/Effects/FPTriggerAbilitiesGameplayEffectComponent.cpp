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
		EventData.EventTag = FFPGAGlobalTags::Get().Misc_Dummy;
		EventData.ContextHandle = ActiveGE.Spec.GetContext();

		// FGameplayAbilitySpecHandle Handle = UFPGAGameplayAbilitiesLibrary::GiveAbilityAndActivateOnce(ASC, Ability, EventData);
		// if (Handle.IsValid())
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Saving granted effect %s"), *Handle.ToString());
		// 	ActiveGE.GrantedAbilityHandles.Add(Handle);
		//
		// 	ASC->SetRemoveAbilityOnEnd(Handle);
		// }


		// FGameplayAbilitySpecHandle Handle = UFPGAGameplayAbilitiesLibrary::GiveAbility(ASC, Ability);
		FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Ability.GetDefaultObject(), 1, -1);
		Spec.bActivateOnce = true;
		Spec.GameplayEffectHandle = ActiveGE.Handle;
		ASC->GiveAbilityAndActivateOnce(Spec, &EventData);

		// FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		//
		// if (Handle.IsValid())
		// {
		// 	// ActiveGE.GrantedAbilityHandles.Add(Handle);
		//
		// 	FGameplayEventData EventData;
		// 	EventData.ContextHandle = ActiveGE.Spec.GetContext();
		//
		//
		// 	// ASC->SetRemoveAbilityOnEnd(Handle);
		//
		// 	// if (UGameplayAbility* GrantedAbility = UFPGAGameplayAbilitiesLibrary::ActivateAbilityWithEvent(ASC, Handle, EventData))
		// 	// {
		// 	// 	ASC->SetRemoveAbilityOnEnd(GrantedAbility->GetCurrentAbilitySpecHandle());
		// 	// }
		// }
	}

	return true;
}
