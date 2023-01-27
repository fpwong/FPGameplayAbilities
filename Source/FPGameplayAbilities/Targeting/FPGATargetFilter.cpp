// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGATargetFilter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPGameplayAbilities/FPGASettings.h"

bool FFPGATargetFilter::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return true;
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

	for (FFPGATargetFilter* Filter : GetStaticFilters())
	{
		if (!Filter->DoesFilterPass(SourceActor, TargetActor))
		{
			return false;
		}
	}

	// if (StaticFilters.Num())
	// {
	// 	if (UDataTable* DataTable = GetDefault<UFPGASettings>()->StaticTargetFiltersTable.LoadSynchronous())
	// 	{
	// 		for (const FGameplayTag& FilterRowName : StaticFilters)
	// 		{
	// 			if (FFPGATargetFilter* Filter = DataTable->FindRow<FFPGATargetFilter>(FilterRowName.GetTagName(), TEXT("FFPGATargetFilter::DoesFilterPass")))
	// 			{
	// 				if (!Filter->DoesFilterPass(SourceActor, TargetActor))
	// 				{
	// 					return false;
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	return true;
}

TArray<FFPGATargetFilter*> FFPGATargetFilter::GetStaticFilters() const
{
	TArray<FFPGATargetFilter*> OutFilters;
	if (StaticFilters.Num())
	{
		if (UDataTable* DataTable = GetDefault<UFPGASettings>()->StaticTargetFiltersTable.LoadSynchronous())
		{
			for (const FGameplayTag& FilterRowName : StaticFilters)
			{
				if (FFPGATargetFilter* Filter = DataTable->FindRow<FFPGATargetFilter>(FilterRowName.GetTagName(), TEXT("FFPGATargetFilter::DoesFilterPass")))
				{
					OutFilters.Add(Filter);
				}
			}
		}
	}

	return OutFilters;
}

void FFPGATargetFilter::PrintFilter() const
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *TargetTagQuery.GetDescription());

	if (StaticFilters.Num())
	{
		if (UDataTable* DataTable = GetDefault<UFPGASettings>()->StaticTargetFiltersTable.LoadSynchronous())
		{
			for (const FGameplayTag& FilterRowName : StaticFilters)
			{
				UE_LOG(LogTemp, Warning, TEXT("Child filter %s"), *FilterRowName.GetTagName().ToString());
				if (FFPGATargetFilter* Filter = DataTable->FindRow<FFPGATargetFilter>(FilterRowName.GetTagName(), TEXT("FFPGATargetFilter::DoesFilterPass")))
				{
					UE_LOG(LogTemp, Warning, TEXT("\t%s"), *Filter->TargetTagQuery.GetDescription());
				}
			}
		}
	}
}

//~~ FFPGATargetFilterCallbacks

void FFPGATargetFilterValidation::BindToActor(const FFPGATargetFilter& InFilter, AActor* InSourceActor, AActor* InTargetActor)
{
	Filter = InFilter;
	SourceActor = InSourceActor;
	TargetActor = InTargetActor;

	if (!TargetActor)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!AbilitySystem)
	{
		return;
	}

	TArray<FGameplayTag> TagDictionary;

	{
		FGameplayTagQueryExpression Query;
		Filter.TargetTagQuery.GetQueryExpr(Query);

		TArray<uint8> Tokens;
		Query.EmitTokens(Tokens, TagDictionary);
	}

	for (FFPGATargetFilter* StaticFilter : InFilter.GetStaticFilters())
	{
		FGameplayTagQueryExpression Query;
		StaticFilter->TargetTagQuery.GetQueryExpr(Query);

		TArray<uint8> Tokens;
		Query.EmitTokens(Tokens, TagDictionary);
	}

	TSet<FGameplayTag> TagSet(TagDictionary);
	for (const FGameplayTag& Tag : TagSet)
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
