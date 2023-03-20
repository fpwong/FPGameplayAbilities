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

	// if (!Query.IsEmpty() && !TagRelationshipMapping)
	// {
	// 	return true;
	// }

	FGameplayTagContainer TargetTags;
	UFPGAGameplayAbilitiesLibrary::FillRelationshipTags(TargetTags, SourceActor, TargetActor);
	TargetAbilitySystem->GetOwnedGameplayTags(TargetTags);

	FGameplayTagContainer AllBlockedTags = BlockedTags;
	FGameplayTagContainer AllRequiredTags = RequiredTags;

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(TagGroups, &AllRequiredTags, &AllBlockedTags);
	}

	if (TargetTags.HasAny(AllBlockedTags))
	{
		return false;
	}

	if (!TargetTags.HasAll(AllRequiredTags))
	{
		return false;
	}

	return true;
}

// Binding

void FFPTargetFilterObserver_GameplayTag::OnFilterTagChanged(FGameplayTag Tag, int NewCount)
{
	CheckResultChanged();
}

FFPTargetFilterObserver_GameplayTag::~FFPTargetFilterObserver_GameplayTag()
{
	bCurrentResult = false;

	if (!TargetActorPtr.IsValid())
	{
		return;
	}

	if (!TagChangedDelegates.Num())
	{
		return;
	}

	if (UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActorPtr.Get()))
	{
		for (const auto& Kvp : TagChangedDelegates)
		{
			AbilitySystem->UnregisterGameplayTagEvent(Kvp.Value, Kvp.Key, EGameplayTagEventType::NewOrRemoved);
		}
	}

	TagChangedDelegates.Empty();
}


FFPTargetFilterObserver* UFPTargetFilterTask_GameplayTag::MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	FFPTargetFilterObserver_GameplayTag* NewBinding = new FFPTargetFilterObserver_GameplayTag();
	NewBinding->Init(FilterTask, SourceActor, TargetActor);
	return NewBinding;
}

void FFPTargetFilterObserver_GameplayTag::Init(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	FFPTargetFilterObserver::Init(FilterTask, SourceActor, TargetActor);

	if (!TargetActor)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!AbilitySystem)
	{
		return;
	}

	FGameplayTagContainer TagSet = Cast<UFPTargetFilterTask_GameplayTag>(Filter)->GetAllRelatedTags();

	for (const FGameplayTag& Tag : TagSet)
	{
		FDelegateHandle Handle = AbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddRaw(this, &FFPTargetFilterObserver_GameplayTag::OnFilterTagChanged);
		TagChangedDelegates.Add(Tag, Handle);
	}

	bCurrentResult = Filter->DoesFilterPass(SourceActor, TargetActor);
}


FGameplayTagContainer UFPTargetFilterTask_GameplayTag::GetAllRelatedTags()
{
	FGameplayTagContainer TagContainer;

	TagContainer.AppendTags(BlockedTags);
	TagContainer.AppendTags(RequiredTags);

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(TagGroups, &TagContainer, &TagContainer);
	}

	return TagContainer;
}

