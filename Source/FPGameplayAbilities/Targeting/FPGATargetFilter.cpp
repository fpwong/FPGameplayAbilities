// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGATargetFilter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FPTargetFilterPreset.h"
#include "FPTargetFilterTaskSetObserver.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPGameplayAbilities/Core/FPTagRelationshipMapping.h"

// #include "FPGameplayAbilities/FPGASettings.h"

// void FFPGATargetFilter::Init()
// {
// 	// try build the query!
// 	if (CachedTagQuery.IsEmpty())
// 	{
// 		FGameplayTagQueryExpression RootExpr;
// 		RootExpr.AllExprMatch();
//
// 		if (!TargetTagQuery.IsEmpty())
// 		{
// 			FGameplayTagQueryExpression Expr;
// 			TargetTagQuery.GetQueryExpr(Expr);
// 			RootExpr.AddExpr(Expr);
// 		}
//
// 		if (TagRelationshipMapping)
// 		{
// 			TagRelationshipMapping->BuildTagQuery(FilterTags, RootExpr);
// 		}
//
// 		CachedTagQuery.Build(RootExpr);
//
// 		UE_LOG(LogTemp, Warning, TEXT("IS THIS WORKING %s"), *CachedTagQuery.GetDescription());
// 	}
// }

// UFPTagRelationshipMapping* IFPTagFilterInterface::GetTagRelationshipMapping_Implementation()
// {
// 	return nullptr;
// }

void FFPGATargetFilter::AddAdditionalTags(const FGameplayTagContainer& AdditionalTags)
{
	// FGameplayTagQueryExpression Expr;
	// CachedTagQuery.GetQueryExpr(Expr);

	TagGroups.AppendTags(AdditionalTags);

	// if (TagRelationshipMapping)
	// {
	// 	TagRelationshipMapping->BuildTagQuery(AdditionalTags, Expr);
	// }
}

bool FFPGATargetFilter::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return true;
	}

	UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!TargetAbilitySystem)
	{
		return false;
	}
	
	// if (!Query.IsEmpty() && !TagRelationshipMapping)
	// {
	// 	return true;
	// }

	// FGameplayTagContainer TargetTags;
	// UFPGAGameplayAbilitiesLibrary::FillRelationshipTags(TargetTags, SourceActor, TargetActor);
	// TargetAbilitySystem->GetOwnedGameplayTags(TargetTags);
	//
	// // if (!Query.IsEmpty())
	// // {
	// // 	if (!Query.Matches(TargetTags))
	// // 	{
	// // 		return false;
	// // 	}
	// // }
	//
	// FGameplayTagContainer AllBlockedTags = BlockedTags; 
	// FGameplayTagContainer AllRequiredTags = RequiredTags; 
	//
	// if (TagRelationshipMapping)
	// {
	// 	TagRelationshipMapping->GetRequiredAndBlockedActivationTags(TagGroups, &AllRequiredTags, &AllBlockedTags);
	// 	// TArray<TSharedRef<FFPTagRelationship>> TagRelationships;
	// 	// TagRelationshipMapping->GetTagRelationships(FilterTags, TagRelationships);
	// 	//
	// 	// for (TSharedRef<FFPTagRelationship> TagRelationship : TagRelationships)
	// 	// {
	// 	// 	if (!TagRelationship->TagQuery.Matches(TargetTags))
	// 	// 	{
	// 	// 		return false;
	// 	// 	}
	// 	// }
	// }
	//
	// if (TargetTags.HasAny(AllBlockedTags))
	// {
	// 	return false;
	// }
	//
	// if (!TargetTags.HasAll(AllRequiredTags))
	// {
	// 	return false;
	// }

	// if (!RequiredEffectQuery.IsEmpty() || RequiredEffectQuery.CustomMatchDelegate_BP.IsBound() || RequiredEffectQuery.CustomMatchDelegate.IsBound())
	// {
	// 	if (AbilitySystem->GetActiveEffects(RequiredEffectQuery).Num() == 0)
	// 		return false;
	// }
	//
	// if (!BlockedEffectQuery.IsEmpty() || BlockedEffectQuery.CustomMatchDelegate_BP.IsBound() || BlockedEffectQuery.CustomMatchDelegate.IsBound())
	// {
	// 	if (AbilitySystem->GetActiveEffects(BlockedEffectQuery).Num() > 0)
	// 		return false;
	// }

	// for (FFPGATargetFilter* Filter : GetStaticFilters())
	// {
	// 	if (!Filter->DoesFilterPass(SourceActor, TargetActor))
	// 	{
	// 		return false;
	// 	}
	// }

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

	for (UFPTargetFilterPreset* TargetingPreset : TargetingPresets)
	{
		UE_LOG(LogTemp, Warning, TEXT("Checking preset"));
		if (!TargetingPreset->DoesFilterPass(SourceActor, TargetActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Preset failed"));
			return false;
		}
	}

	return true;
}

// TArray<FFPGATargetFilter*> FFPGATargetFilter::GetStaticFilters() const
// {
// 	TArray<FFPGATargetFilter*> OutFilters;
// 	if (StaticFilters.Num())
// 	{
// 		if (UDataTable* DataTable = GetDefault<UFPGASettings>()->StaticTargetFiltersTable.LoadSynchronous())
// 		{
// 			for (const FGameplayTag& FilterRowName : StaticFilters)
// 			{
// 				if (FFPGATargetFilter* Filter = DataTable->FindRow<FFPGATargetFilter>(FilterRowName.GetTagName(), TEXT("FFPGATargetFilter::DoesFilterPass")))
// 				{
// 					OutFilters.Add(Filter);
// 				}
// 			}
// 		}
// 	}
//
// 	return OutFilters;
// }

void FFPGATargetFilter::PrintFilter() const
{
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *TargetTagQuery.GetDescription());

	// if (StaticFilters.Num())
	// {
	// 	if (UDataTable* DataTable = GetDefault<UFPGASettings>()->StaticTargetFiltersTable.LoadSynchronous())
	// 	{
	// 		for (const FGameplayTag& FilterRowName : StaticFilters)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("Child filter %s"), *FilterRowName.GetTagName().ToString());
	// 			if (FFPGATargetFilter* Filter = DataTable->FindRow<FFPGATargetFilter>(FilterRowName.GetTagName(), TEXT("FFPGATargetFilter::DoesFilterPass")))
	// 			{
	// 				UE_LOG(LogTemp, Warning, TEXT("\t%s"), *Filter->TargetTagQuery.GetDescription());
	// 			}
	// 		}
	// 	}
	// }
}

FGameplayTagContainer FFPGATargetFilter::GetAllRelatedTags()
{
	FGameplayTagContainer TagContainer;

	TagContainer.AppendTags(BlockedTags);
	TagContainer.AppendTags(RequiredTags);

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(TagGroups, &TagContainer, &TagContainer);
	}

	return TagContainer;

	// TArray<FGameplayTag> TagDictionary;
	// UFPGAGameplayAbilitiesLibrary::GetQueryTags(GetTagQuery(), TagDictionary);

	// if (TagRelationshipMapping)
	// {
	// 	TArray<TSharedRef<FFPTagRelationship>> TagRelationships;
	// 	// TagRelationshipMapping->GetAllQueryTags(FilterTags, TagDictionary);
	// }

	// return TSet<FGameplayTag>(TagDictionary);
	// return TSet<FGameplayTag>(TagDictionary);
}

void FFPTargetFilterValidation::Bind(const FFPGATargetFilter& InFilter, AActor* InSourceActor, AActor* InTargetActor)
{
	Filter = InFilter;
	SourceActor = InSourceActor;
	TargetActor = InTargetActor;

	for (UFPTargetFilterPreset* TargetingPreset : InFilter.TargetingPresets)
	{
		// TODO
		// TSharedPtr<FFPOngoingTargetFilterTaskset> Binding = MakeShared<FFPOngoingTargetFilterTaskset>();
		// Binding->Bind(TargetingPreset, InSourceActor, InTargetActor);
		// Binding->OnResultChanged.AddRaw(this, &FFPTargetFilterValidation::HandleResultChanged);
		// PresetBindings.Add(Binding);
	}

	bCurrentResult = Filter.DoesFilterPass(SourceActor.Get(), TargetActor.Get());
	UE_LOG(LogTemp, Warning, TEXT("Current result %d"), bCurrentResult);
}

void FFPTargetFilterValidation::Clear()
{
	bCurrentResult = false;

	if (!TargetActor.IsValid())
	{
		return;
	}
}

void FFPTargetFilterValidation::HandleResultChanged(AActor* Actor, bool)
{
	bool bNewResult = Filter.DoesFilterPass(SourceActor.Get(), TargetActor.Get());
	UE_LOG(LogTemp, Warning, TEXT("Current result %d"), bNewResult);
	if (bCurrentResult != bNewResult)
	{
		OnFilterResultChanged.Broadcast(Actor, bNewResult);
		bCurrentResult = bNewResult;
	}
}
