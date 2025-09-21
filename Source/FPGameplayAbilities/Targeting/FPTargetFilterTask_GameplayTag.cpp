// Fill out your copyright notice in the Description page of Project Settings.


#include "FPTargetFilterTask_GameplayTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FPTargetFilterObserver.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPGameplayAbilities/Utils/FPGAMiscUtils.h"

bool FFPTargetFilterTask_GameplayTag::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags) const
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

	if (!CheckTagRequirements(SourceActor, TargetActor, TargetRequirements, OutFailureTags))
	{
		return false;
	}

	if (!CheckTagRequirements(TargetActor, SourceActor, SourceRequirements, OutFailureTags))
	{
		return false;
	}

	return true;
}

bool FFPTargetFilterTask_GameplayTag::CheckTagRequirements(const AActor* Source, const AActor* Target, const FGameplayTagRequirements& Requirements, FGameplayTagContainer* OutFailureTags) const
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

	if (!OutFailureTags)
	{
		return Requirements.RequirementsMet(TargetTags);
	}

	const FGameplayTagContainer BlockedTags = TargetTags.Filter(Requirements.IgnoreTags);
	if (BlockedTags.Num())
	{
		OutFailureTags->AppendTags(BlockedTags);
		return false;
	}

	const FGameplayTagContainer MissingRequiredTags = UFPGAMiscUtils::GetMissingTags(Requirements.RequireTags, TargetTags);
	if (MissingRequiredTags.Num())
	{
		OutFailureTags->AppendTags(MissingRequiredTags);
		return false;
	}

	return true;
}

void FFPTargetFilterTask_GameplayTag::BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const
{
	BindToActor(Observer, SourceActor, SourceRequirements);
	BindToActor(Observer, TargetActor, TargetRequirements);
}

void FFPTargetFilterTask_GameplayTag::BindToActor(FFPTargetFilterObserver& Observer, AActor* Actor, const FGameplayTagRequirements& Requirements) const
{
	if (!Actor)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if (!AbilitySystem)
	{
		return;
	}

	FGameplayTagContainer AllTags;
	AllTags.AppendTags(Requirements.RequireTags);
	AllTags.AppendTags(Requirements.IgnoreTags);

	for (const FGameplayTag& Tag : AllTags)
	{
		FDelegateHandle Handle = AbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddLambda([&Observer](const FGameplayTag, int32)
		{
			Observer.CheckResultChanged();
		});

		Observer.AddCleanup([WeakSys = TWeakObjectPtr(AbilitySystem), Handle, Tag]
		{
			if (WeakSys.IsValid())
			{
				WeakSys->UnregisterGameplayTagEvent(Handle, Tag, EGameplayTagEventType::NewOrRemoved);
			}
		});
	}
}

bool FFPTargetFilterTask_Relationship::DoesFilterPass(const AActor* SourceActor, const AActor* Target, FGameplayTagContainer* OutFailureTags) const
{
	if (!Target)
	{
		return Requirements.IsEmpty();
	}

	FGameplayTagContainer TargetTags;
	UFPGAGameplayAbilitiesLibrary::FillRelationshipTags(TargetTags, SourceActor, Target);

	if (!OutFailureTags)
	{
		return TargetTags.HasAny(Requirements);
	}

	const FGameplayTagContainer MissingRequiredTags = UFPGAMiscUtils::GetMissingTags(Requirements, TargetTags);
	if (MissingRequiredTags.Num())
	{
		OutFailureTags->AppendTags(MissingRequiredTags);
		return false;
	}

	return true;
}
