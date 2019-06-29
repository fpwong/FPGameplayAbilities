// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/FPGAAbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

void UFPGAGameplayAbility::OnCooldownFinished(const FGameplayEffectRemovalInfo& RemovalInfo)
{
	OnAbilityCooldown.Broadcast(this);
}

//////////////
// Cooldown //
//////////////
void UFPGAGameplayAbility::FPGAApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownGameplayEffectClass, GetAbilityLevel(Handle, ActorInfo));
		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			FGameplayEffectSpec& Spec = *SpecHandle.Data.Get();
			AdjustCooldownSpec(Spec);
			CooldownEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
		 //= ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, CooldownGE, GetAbilityLevel(Handle, ActorInfo));
	}
}

bool UFPGAGameplayAbility::HasCooldown()
{
	/*const FGameplayTagContainer* CooldownTags = GetCooldownTags();
	if (CooldownTags)
	{
		return CooldownTags->Num() > 0;
	}*/

	return GetCooldownGameplayEffect() != nullptr;
}

bool UFPGAGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (GetCooldownGameplayEffect())
	{
		const FActiveGameplayEffect* ActiveGE = ActorInfo->AbilitySystemComponent->GetActiveGameplayEffect(CooldownEffectHandle);
		return ActiveGE == nullptr;
	}

	return true;

	const FGameplayTagContainer* CooldownTags = GetCooldownTags();
	if (CooldownTags)
	{
		if (CooldownTags->Num() > 0)
		{
			UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
			check(AbilitySystemComponent != nullptr);
			if (AbilitySystemComponent->HasAnyMatchingGameplayTags(*CooldownTags))
			{
				const FGameplayTag& CooldownTag = UAbilitySystemGlobals::Get().ActivateFailCooldownTag;

				if (OptionalRelevantTags && CooldownTag.IsValid())
				{
					OptionalRelevantTags->AddTag(CooldownTag);
				}

				return false;
			}
		}
	}
	return true;
}

float UFPGAGameplayAbility::GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (GetCooldownGameplayEffect())
	{
		if (const FActiveGameplayEffect* Effect = ActorInfo->AbilitySystemComponent->GetActiveGameplayEffect(CooldownEffectHandle))
		{
			return Effect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		}
	}

	return 0.f;
}

void UFPGAGameplayAbility::GetCooldownTimeRemainingAndDuration(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const
{
	if (GetCooldownGameplayEffect())
	{
		if (const FActiveGameplayEffect* Effect = ActorInfo->AbilitySystemComponent->GetActiveGameplayEffect(CooldownEffectHandle))
		{
			TimeRemaining = Effect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			CooldownDuration = Effect->GetDuration();
		}
	}
}

void UFPGAGameplayAbility::OnGameplayEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	UGameplayEffect* Effect = GetCooldownGameplayEffect();

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();

	if (Effect == AbilitySystem->GetGameplayEffectDefForHandle(ActiveHandle))
	{
		AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.Remove(MyDelHandle);
		AbilitySystem->OnGameplayEffectRemoved_InfoDelegate(ActiveHandle)->AddUObject(this, &UFPGAGameplayAbility::OnCooldownFinished);
	}
}

void UFPGAGameplayAbility::CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	FPGAApplyCooldown(Handle, ActorInfo, ActivationInfo);

	ApplyCost(Handle, ActorInfo, ActivationInfo);
}

bool UFPGAGameplayAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UGameplayEffect* Effect = GetCooldownGameplayEffect();
	if (Effect != nullptr)
	{
		UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
		if (AbilitySystem != nullptr)
		{
			MyDelHandle = AbilitySystem->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UFPGAGameplayAbility::OnGameplayEffectApplied);
		}
	}

	bool bCommitted = UGameplayAbility::CommitAbility(Handle, ActorInfo, ActivationInfo);

	OnAbilityCommitted.Broadcast(this);

	return bCommitted;
}

bool UFPGAGameplayAbility::CommitAbilityCooldown(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const bool ForceCooldown)
{
	if (UAbilitySystemGlobals::Get().ShouldIgnoreCooldowns())
	{
		return true;
	}

	if (!ForceCooldown)
	{
		// Last chance to fail (maybe we no longer have resources to commit since we after we started this ability activation)
		if (!CheckCooldown(Handle, ActorInfo))
		{
			return false;
		}
	}

	FPGAApplyCooldown(Handle, ActorInfo, ActivationInfo);
	return true;
}

void UFPGAGameplayAbility::BlockAndCancelTags()
{
	UAbilitySystemComponent* Comp = GetActorInfo().AbilitySystemComponent.Get();
	Comp->ApplyAbilityBlockAndCancelTags(AbilityTags, this, true, BlockAbilitiesWithTag, true, CancelAbilitiesWithTag);
}

void UFPGAGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	OnAbilityActivated.Broadcast(this);
	UGameplayAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UFPGAGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UGameplayAbility::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	OnAbilityEnded.Broadcast(this);
}

bool UFPGAGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		check(AbilitySystemComponent != nullptr);

		bool bCanApplyAttributeModifiers = true;

		FGameplayEffectSpec Spec(CostGE, MakeEffectContext(Handle, ActorInfo), GetAbilityLevel(Handle, ActorInfo));
		AdjustCostSpec(Spec);
		Spec.CalculateModifierMagnitudes();

		for (int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
		{
			const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIdx];
			const FModifierSpec& ModSpec = Spec.Modifiers[ModIdx];

			// It only makes sense to check additive operators
			if (ModDef.ModifierOp == EGameplayModOp::Additive)
			{
				if (!ModDef.Attribute.IsValid())
				{
					continue;
				}

				UAttributeSet* Set = UFPGAGameplayAbilitiesLibrary::FindAttributeSetOfClass(AbilitySystemComponent, ModDef.Attribute.GetAttributeSetClass());

				float CurrentValue = ModDef.Attribute.GetNumericValueChecked(Set);
				float CostValue = ModSpec.GetEvaluatedMagnitude();

				if (CurrentValue + CostValue < 0.f)
				{
					bCanApplyAttributeModifiers = false;
				}
			}
		}

		if (!bCanApplyAttributeModifiers)
		{
			const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

			if (OptionalRelevantTags && CostTag.IsValid())
			{
				OptionalRelevantTags->AddTag(CostTag);
			}
			return false;
		}
	}
	return true;
}

void UFPGAGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostGameplayEffectClass, GetAbilityLevel(Handle, ActorInfo));
		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			FGameplayEffectSpec& Spec = *SpecHandle.Data.Get();
			AdjustCostSpec(Spec);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error: Spec handle or data is invalid"));
		}
	}
}
