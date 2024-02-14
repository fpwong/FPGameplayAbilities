#include "AbilitySystem/FPGATypes.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "Misc/FPGameplayValueRow.h"

void FFPGAAbilityReadyBinding::Clear()
{
	OnAbilityReady.Clear();
	BoundAbility.Reset();

	// if (BoundAbilitySystem.IsValid())
	// {
	// 	for (FActiveGameplayEffectHandle& BoundHandle : BoundHandles)
	// 	{
	// 		BoundCooldownDelegates.Add(BoundAbilitySystem.Get()->OnGameplayEffectRemoved_InfoDelegate(BoundHandle)->RemoveAll(this));
	// 	}
	// }

	BoundCooldownDelegates.Reset();
}

void FFPGAAbilityReadyBinding::BindToGameplayAbility(UGameplayAbility* Ability)
{
	if (!Ability)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = Ability->GetAbilitySystemComponentFromActorInfo();
	if (!ensure(AbilitySystem))
	{
		return;
	}

	if (BoundAbility == Ability)
	{
		return;
	}

	BoundAbility = Ability;
	
	// UE_LOG(LogTemp, Warning, TEXT("bound to ability?"));

	// bind to cooldown
	if (const FGameplayTagContainer* CooldownTags = Ability->GetCooldownTags())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Found cooldowns"));
		FGameplayEffectQuery CooldownQuery;
		CooldownQuery.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAllTags(*CooldownTags);

		TArray<FActiveGameplayEffectHandle> BlockingEffects = AbilitySystem->GetActiveEffects(CooldownQuery);

		for (FActiveGameplayEffectHandle Handle : BlockingEffects)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Bound to handle!"));
			BoundHandles.Add(Handle);
			BoundCooldownDelegates.Add(AbilitySystem->OnGameplayEffectRemoved_InfoDelegate(Handle)->AddRaw(this, &FFPGAAbilityReadyBinding::OnCooldownEffectRemoved));
		}
	}

	Ability->OnGameplayAbilityEnded.AddRaw(this, &FFPGAAbilityReadyBinding::OnAbilityEnded);

	// bind to cost
}

void FFPGAAbilityReadyBinding::OnCooldownEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo)
{
	// UE_LOG(LogTemp, Warning, TEXT("cooldown removed!"));
	CheckAbilityReady();
}

void FFPGAAbilityReadyBinding::CheckAbilityReady()
{
	if (BoundAbility.IsValid())
	{
		if (UFPGAGameplayAbilitiesLibrary::CanActivateAbility(BoundAbility->GetAbilitySystemComponentFromActorInfo(), BoundAbility.Get()))
		{
			UGameplayAbility* Ability = BoundAbility.Get();
			BoundAbility.Reset();
			OnAbilityReady.Broadcast(Ability);
		}
	}
}

void FFPGAAbilityReadyBinding::OnAbilityEnded(UGameplayAbility* GameplayAbility)
{
	// UE_LOG(LogTemp, Warning, TEXT("Ability ended!"));
	if (BoundAbility.IsValid())
	{
		GameplayAbility->OnGameplayAbilityEnded.RemoveAll(this);
		CheckAbilityReady();
	}
}

void FFPGameplayEffectParameters::ApplyToGameplayEffectSpec(UAbilitySystemComponent* ASC, TSharedPtr<FGameplayEffectSpec> Spec) const
{
	// apply static set by caller magnitudes
	for (const FFPSetByCallerMagnitude& Elem : SetByCallerParameters)
	{
		Spec->SetSetByCallerMagnitude(Elem.DataTag, Elem.Magnitude);
	}

	for (const FFPTableGameplayEffectParameters& Param : TableParameters)
	{
		// grab gameplay tags from GameplayEffect
		TArray<FGameplayTag> SetByCallerTags = UFPGAGameplayAbilitiesLibrary::GetSetByCallerTagsFromGameplayEffect(Spec->Def);

		const FGameplayTagContainer& EffectTags = Spec->CapturedSourceTags.GetSpecTags();

		// look for the tag in the data table
		for (const FGameplayTag& Tag : SetByCallerTags)
		{
			float TransformedValue = 0.0f;
			if (UFPGameplayValueHelpers::GetTransformedValueFromTable(Param.DataTable, ASC, Tag, EffectTags, TransformedValue))
			{
				UE_LOG(LogTemp, Warning, TEXT("Set %s to %f"), *Tag.GetTagName().ToString(), TransformedValue);
				Spec->SetSetByCallerMagnitude(Tag, TransformedValue);
			}
		}

		// apply effect period if it is set
		if (Param.EffectPeriodTag.IsValid())
		{
			float TransformedValue = 0.0f;
			if (UFPGameplayValueHelpers::GetTransformedValueFromTable(Param.DataTable, ASC, Param.EffectPeriodTag, EffectTags, TransformedValue))
			{
				Spec->Period = TransformedValue;
			}
		}
	}
}
