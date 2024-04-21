// Copyright 2020 Dan Kestranek.

#include "AbilitySystem/Tasks/FPGAAsyncTask_CooldownChanged.h"
#include "Kismet/KismetSystemLibrary.h"

UFPGAAsyncTask_CooldownChanged * UFPGAAsyncTask_CooldownChanged::ListenForCooldownChange(UAbilitySystemComponent * AbilitySystemComponent, FGameplayTagContainer InCooldownTags, bool InUseServerCooldown)
{
	UFPGAAsyncTask_CooldownChanged* ListenForCooldownChange = NewObject<UFPGAAsyncTask_CooldownChanged>();
	ListenForCooldownChange->ASC = AbilitySystemComponent;
	ListenForCooldownChange->CooldownTags = InCooldownTags;
	ListenForCooldownChange->UseServerCooldown = InUseServerCooldown;

	if (!IsValid(AbilitySystemComponent) || InCooldownTags.Num() < 1)
	{
		ListenForCooldownChange->EndTask();
		return nullptr;
	}

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenForCooldownChange, &UFPGAAsyncTask_CooldownChanged::OnActiveGameplayEffectAddedCallback);

	TArray<FGameplayTag> CooldownTagArray;
	InCooldownTags.GetGameplayTagArray(CooldownTagArray);
	
	for (FGameplayTag CooldownTag : CooldownTagArray)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenForCooldownChange, &UFPGAAsyncTask_CooldownChanged::CooldownTagChanged);
	}

	return ListenForCooldownChange;
}

void UFPGAAsyncTask_CooldownChanged::Activate()
{
	FGameplayEffectQuery Query;
	Query.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAllTags(CooldownTags);
	TArray<TPair<float, float>> RemainingTimes = ASC->GetActiveEffectsTimeRemainingAndDuration(Query);

	// UE_LOG(LogTemp, Warning, TEXT("Checking for cd tags %s"), *CooldownTags.ToStringSimple());

	if (RemainingTimes.Num() > 0)
	{
		// UE_LOG(LogTemp, Warning, TEXT("\tAbility already on cooldown!"));
		OnCooldownBegin.Broadcast(CooldownTags.First(), RemainingTimes[0].Key, RemainingTimes[0].Value);
	}
}

void UFPGAAsyncTask_CooldownChanged::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);

		TArray<FGameplayTag> CooldownTagArray;
		CooldownTags.GetGameplayTagArray(CooldownTagArray);

		for (FGameplayTag CooldownTag : CooldownTagArray)
		{
			ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
}

void UFPGAAsyncTask_CooldownChanged::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent * Target, const FGameplayEffectSpec & SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);
	
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	TArray<FGameplayTag> CooldownTagArray;
	CooldownTags.GetGameplayTagArray(CooldownTagArray);

	for (FGameplayTag CooldownTag : CooldownTagArray)
	{
		if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
		{
			float TimeRemaining = 0.0f;
			float Duration = 0.0f;
			// Expecting cooldown tag to always be first tag
			FGameplayTagContainer CooldownTagContainer(GrantedTags.GetByIndex(0));
			GetCooldownRemainingForTag(CooldownTagContainer, TimeRemaining, Duration);

			OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			
			// if (ASC->GetOwnerRole() == ROLE_Authority)
			// {
			// 	// Player is Server
			// 	OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			// }
			// else if (!UseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			// {
			// 	// Client using predicted cooldown
			// 	OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			// }
			// else if (UseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr)
			// {
			// 	// Client using Server's cooldown. This is Server's corrective cooldown GE.
			// 	OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			// }
			// else if (UseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			// {
			// 	// Client using Server's cooldown but this is predicted cooldown GE.
			// 	// This can be useful to gray out abilities until Server's cooldown comes in.
			// 	OnCooldownBegin.Broadcast(CooldownTag, -1.0f, -1.0f);
			// }
		}
	}
}

void UFPGAAsyncTask_CooldownChanged::CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		OnCooldownEnd.Broadcast(CooldownTag, -1.0f, -1.0f);
	}
}

bool UFPGAAsyncTask_CooldownChanged::GetCooldownRemainingForTag(FGameplayTagContainer InCooldownTags, float & TimeRemaining, float & CooldownDuration)
{
	if (IsValid(ASC) && InCooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = ASC->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}
