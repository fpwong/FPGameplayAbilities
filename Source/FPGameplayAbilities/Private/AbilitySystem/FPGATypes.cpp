#include "FPGATypes.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

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
