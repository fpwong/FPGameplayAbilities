// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGATargetFilter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

bool FFPGATargetFilter::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!AbilitySystem)
	{
		return false;
	}

	if (!TargetTagQuery.IsEmpty())
	{
		FGameplayTagContainer TargetTags;

		UFPGAGameplayAbilitiesLibrary::FillRelationshipTags(TargetTags, SourceActor, TargetActor);

		AbilitySystem->GetOwnedGameplayTags(TargetTags);
		if (!TargetTagQuery.Matches(TargetTags))
		{
			return false;
		}
	}

	if (!RequiredEffectQuery.IsEmpty() || RequiredEffectQuery.CustomMatchDelegate_BP.IsBound() || RequiredEffectQuery.CustomMatchDelegate.IsBound())
	{
		if (AbilitySystem->GetActiveEffects(RequiredEffectQuery).Num() == 0)
			return false;
	}

	if (!BlockedEffectQuery.IsEmpty() || BlockedEffectQuery.CustomMatchDelegate_BP.IsBound() || BlockedEffectQuery.CustomMatchDelegate.IsBound())
	{
		if (AbilitySystem->GetActiveEffects(BlockedEffectQuery).Num() > 0)
			return false;
	}

	return true;
}

//~~ FFPGATargetFilterCallbacks

void FFPGATargetFilterValidation::BindToActor(const FFPGATargetFilter& InFilter, AActor* InSourceActor, AActor* InTargetActor)
{
	Filter = InFilter;
	SourceActor = InSourceActor;
	TargetActor = InTargetActor;

	if (!SourceActor || !TargetActor)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!AbilitySystem)
	{
		return;
	}

	FGameplayTagQueryExpression Query;
	Filter.TargetTagQuery.GetQueryExpr(Query);

	TArray<uint8> Tokens;
	TArray<FGameplayTag> TagDictionary;
	Query.EmitTokens(Tokens, TagDictionary);

	for (const FGameplayTag& Tag : TagDictionary)
	{
		FDelegateHandle Handle = AbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FFPGATargetFilterValidation::OnFilterTagChanged);
		TagChangedDelegates.Add(Tag, Handle);
	}
}

void FFPGATargetFilterValidation::OnFilterTagChanged(FGameplayTag Tag, int NewCount)
{
	if (!Filter.DoesFilterPass(SourceActor, TargetActor))
	{
		OnFilterFailed.Broadcast();
	}
}

void FFPGATargetFilterValidation::Clear()
{
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		for (const auto& Kvp : TagChangedDelegates)
		{
			AbilitySystem->UnregisterGameplayTagEvent(Kvp.Value, Kvp.Key, EGameplayTagEventType::NewOrRemoved);
		}
	}
}
