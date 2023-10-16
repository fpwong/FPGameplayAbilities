// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGACostMethod.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

UGameplayEffect* UFPGACostMethod_Attribute::GetCostGameplayEffect() const
{
	UGameplayEffect* DynamicCostEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("DynamicCost")));
	DynamicCostEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
	DynamicCostEffect->Modifiers.Append(AbilityCosts);
	return DynamicCostEffect;
}

void UFPGACostMethod_Attribute::ApplyCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		FGameplayEffectContextHandle EffectContext = Ability->MakeEffectContext(Handle, ActorInfo);
		int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
		UAbilitySystemComponent* AbilitySys = ActorInfo->AbilitySystemComponent.Get(); 
		UFPGAGameplayAbilitiesLibrary::ApplyGameplayEffect(CostGE, AbilitySys, AbilitySys, AbilityLevel, EffectContext);
		
		// FGameplayEffectSpec* NewSpec = new FGameplayEffectSpec(CostGE, EffectContext, AbilityLevel);
		// FGameplayEffectSpecHandle SpecHandle(NewSpec);
		// if (SpecHandle.IsValid())
		// {
		// 	
		// 	// ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(CostGE);
		// 	// Ability->ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		// }
	}
}

bool UFPGACostMethod_Attribute::CheckCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	auto CanApplyMods = [](UAbilitySystemComponent* AbilitySys, const UGameplayEffect* GameplayEffect, float Level, const FGameplayEffectContextHandle& EffectContext)
	{
		FGameplayEffectSpec	Spec(GameplayEffect, EffectContext, Level);
		
		Spec.CalculateModifierMagnitudes();
	
		for(int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
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

				float CurrentValue = 0;

				// TODO check whether its worth implementing the fast version of this?
				// const UAttributeSet* Set = AbilitySys->GetAttributeSubobject(ModDef.Attribute.GetAttributeSetClass());
				// CurrentValue = ModDef.Attribute.GetNumericValueChecked(Set);

				bool bOutSuccess = false;
				CurrentValue = UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTags(AbilitySys, ModDef.Attribute, ModDef.SourceTags.RequireTags, ModDef.TargetTags.RequireTags, bOutSuccess);

				float CostValue = ModSpec.GetEvaluatedMagnitude();

				if (CurrentValue + CostValue < 0.f)
				{
					return false;
				}
			}
		}

		return true;
	};
	
	UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	check(AbilitySystemComponent != nullptr);

	// return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE)
	{
		// if (!AbilitySystemComponent->CanApplyAttributeModifiers(CostGE, GetAbilityLevel(Handle, ActorInfo), MakeEffectContext(Handle, ActorInfo)))
		if (!CanApplyMods(AbilitySystemComponent, CostGE, Ability->GetAbilityLevel(Handle, ActorInfo), Ability->MakeEffectContext(Handle, ActorInfo)))
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
