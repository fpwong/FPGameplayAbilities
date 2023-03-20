// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterTask_GameplayTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FPTargetFilterTaskSetObserver.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPGameplayAbilities/Core/FPTagRelationshipMapping.h"

bool UFPTargetFilterTask_GameplayTag::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
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

	const auto CheckTagRequirements = [](const AActor* Source, const AActor* Target, const FGameplayTagRequirements& Requirements)
	{
		if (!Target)
		{
			return Requirements.IsEmpty();
		}

		const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Target);
		if (!TagInterface)
		{
			return false;
		}

		FGameplayTagContainer TargetTags;
		TagInterface->GetOwnedGameplayTags(TargetTags);
		UFPGAGameplayAbilitiesLibrary::FillRelationshipTags(TargetTags, Source, Target);

		return Requirements.RequirementsMet(TargetTags);
	};

	if (!CheckTagRequirements(SourceActor, TargetActor, TargetRequirements))
	{
		return false;
	}

	if (!CheckTagRequirements(TargetActor, SourceActor, SourceRequirements))
	{
		return false;
	}

	return true;

	// if (!Query.IsEmpty() && !TagRelationshipMapping)
	// {
	// 	return true;
	// }

	// FGameplayTagContainer TargetTags;
	// UFPGAGameplayAbilitiesLibrary::FillRelationshipTags(TargetTags, SourceActor, TargetActor);
	// TargetAbilitySystem->GetOwnedGameplayTags(TargetTags);
	//
	// // FGameplayTagContainer AllBlockedTags = BlockedTags;
	// // FGameplayTagContainer AllRequiredTags = RequiredTags;
	// //
	// // if (TagRelationshipMapping)
	// // {
	// // 	TagRelationshipMapping->GetRequiredAndBlockedActivationTags(TagGroups, &AllRequiredTags, &AllBlockedTags);
	// // }
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
	//
	// return true;
}

FFPTargetFilterObserver* UFPTargetFilterTask_GameplayTag::MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	FFPTargetFilterObserver_GameplayTag* NewBinding = new FFPTargetFilterObserver_GameplayTag();
	NewBinding->Init(FilterTask, SourceActor, TargetActor);
	return NewBinding;
}

// Binding

void FFPTargetFilterObserver_GameplayTag::UnbindDelegates(AActor* Actor, const TMap<FGameplayTag, FDelegateHandle>& DelegateHandles)
{
	if (!DelegateHandles.Num())
	{
		return;
	}
	
	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		for (const auto& Kvp : DelegateHandles)
		{
			AbilitySystem->UnregisterGameplayTagEvent(Kvp.Value, Kvp.Key, EGameplayTagEventType::NewOrRemoved);
		}
	}
}

void FFPTargetFilterObserver_GameplayTag::OnFilterTagChanged(FGameplayTag Tag, int NewCount)
{
	CheckResultChanged();
}

FFPTargetFilterObserver_GameplayTag::~FFPTargetFilterObserver_GameplayTag()
{
	bCurrentResult = false;

	if (TargetActorPtr.IsValid())
	{
		UnbindDelegates(TargetActorPtr.Get(), TargetDelegateHandles);
	}

	if (SourceActorPtr.IsValid())
	{
		UnbindDelegates(SourceActorPtr.Get(), SourceDelegateHandles);
	}
	
	// if (!TargetActorPtr.IsValid())
	// {
	// 	return;
	// }
	//
	// if (!TagChangedDelegates.Num())
	// {
	// 	return;
	// }
	//
	// if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActorPtr.Get()))
	// {
	// 	for (const auto& Kvp : TagChangedDelegates)
	// 	{
	// 		AbilitySystem->UnregisterGameplayTagEvent(Kvp.Value, Kvp.Key, EGameplayTagEventType::NewOrRemoved);
	// 	}
	// }
	//
	// TagChangedDelegates.Empty();
}

// FGameplayTagContainer UFPTargetFilterTask_GameplayTag::GetAllRelatedTags()
// {
// 	FGameplayTagContainer TagContainer;
// 	
// 	TagContainer.AppendTags(BlockedTags);
// 	TagContainer.AppendTags(RequiredTags);
//
// 	if (TagRelationshipMapping)
// 	{
// 		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(TagGroups, &TagContainer, &TagContainer);
// 	}
//
// 	return TagContainer;
// }

void FFPTargetFilterObserver_GameplayTag::Init(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	FFPTargetFilterObserver::Init(FilterTask, SourceActor, TargetActor);

	if (auto GameplayTagFilter = Cast<UFPTargetFilterTask_GameplayTag>(Filter))
	{
		BindToActor(SourceActor, GameplayTagFilter->SourceRequirements, SourceDelegateHandles);
		BindToActor(TargetActor, GameplayTagFilter->TargetRequirements, TargetDelegateHandles);
	}

	// if (!TargetActor)
	// {
	// 	return;
	// }
	//
	// UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	// if (!AbilitySystem)
	// {
	// 	return;
	// }
	//
	// // FGameplayTagContainer TagSet = Cast<UFPTargetFilterTask_GameplayTag>(Filter)->GetAllRelatedTags();
	// auto GameplayTagFilter = Cast<UFPTargetFilterTask_GameplayTag>(Filter);
	// FGameplayTagContainer TagSet = GameplayTagFilter->SourceRequirements;
	//
	// for (const FGameplayTag& Tag : TagSet)
	// {
	// 	FDelegateHandle Handle = AbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FFPTargetFilterObserver_GameplayTag::OnFilterTagChanged);
	// 	TagChangedDelegates.Add(Tag, Handle);
	// }
	//
	// bCurrentResult = Filter->DoesFilterPass(SourceActor, TargetActor);
}

void FFPTargetFilterObserver_GameplayTag::BindToActor(AActor* Actor, const FGameplayTagRequirements& Requirements, TMap<FGameplayTag, FDelegateHandle>& OutDelegateHandles)
{
	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if (!AbilitySystem)
	{
		return;
	}

	FGameplayTagContainer AllTags;
	AllTags.AppendTags(Requirements.RequireTags);
	AllTags.AppendTags(Requirements.IgnoreTags);

	for (const FGameplayTag& Tag : Requirements.RequireTags)
	{
		FDelegateHandle Handle = AbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FFPTargetFilterObserver_GameplayTag::OnFilterTagChanged);
		OutDelegateHandles.Add(Tag, Handle);
	}
}
