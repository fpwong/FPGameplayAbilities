// Copyright 2020 Dan Kestranek.

#include "AbilitySystem/Tasks/FPGAAsyncTask_EffectAddedOrRemoved.h"

UFPGAAsyncTask_EffectAddedOrRemoved * UFPGAAsyncTask_EffectAddedOrRemoved::ListenForGameplayEffectAddedOrRemoved(UAbilitySystemComponent * AbilitySystemComponent, FGameplayTag InEffectGameplayTag, bool bInCheckExactTag)
{
	if (!IsValid(AbilitySystemComponent) || !InEffectGameplayTag.IsValid())
	{
		return nullptr;
	}
	
	UFPGAAsyncTask_EffectAddedOrRemoved* ListenForGameplayEffectStackChange = NewObject<UFPGAAsyncTask_EffectAddedOrRemoved>();
	ListenForGameplayEffectStackChange->ASC = AbilitySystemComponent;
	ListenForGameplayEffectStackChange->EffectGameplayTag = InEffectGameplayTag;
	ListenForGameplayEffectStackChange->bCheckExactTag = bInCheckExactTag;

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(ListenForGameplayEffectStackChange, &UFPGAAsyncTask_EffectAddedOrRemoved::OnActiveGameplayEffectAddedCallback);
	AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(ListenForGameplayEffectStackChange, &UFPGAAsyncTask_EffectAddedOrRemoved::OnRemoveGameplayEffectCallback);

	return ListenForGameplayEffectStackChange;
}

void UFPGAAsyncTask_EffectAddedOrRemoved::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
		ASC->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UFPGAAsyncTask_EffectAddedOrRemoved::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec & SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	const bool bIsCorrectTag = bCheckExactTag
		? AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag)
		: AssetTags.HasTag(EffectGameplayTag) || GrantedTags.HasTag(EffectGameplayTag);
	
	if (bIsCorrectTag)
	{
		// ASC->OnGameplayEffectAppliedToSelf(ActiveHandle)->AddUObject(this, &UFPGAAsyncTask_EffectAddedOrRemoved::GameplayEffectAddedOrRemoved);
		OnGameplayEffectAdded.Broadcast(EffectGameplayTag, ActiveHandle);
	}
}

void UFPGAAsyncTask_EffectAddedOrRemoved::OnRemoveGameplayEffectCallback(const FActiveGameplayEffect& EffectRemoved)
{
	FGameplayTagContainer AssetTags;
	EffectRemoved.Spec.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	EffectRemoved.Spec.GetAllGrantedTags(GrantedTags);

	const bool bIsCorrectTag = bCheckExactTag
		? AssetTags.HasTagExact(EffectGameplayTag) || GrantedTags.HasTagExact(EffectGameplayTag)
		: AssetTags.HasTag(EffectGameplayTag) || GrantedTags.HasTag(EffectGameplayTag);
	
	if (bIsCorrectTag)
	{
		// OnGameplayEffectStackChange.Broadcast(EffectGameplayTag, EffectRemoved.Handle, 0, 1);
		OnGameplayEffectRemoved.Broadcast(EffectGameplayTag, EffectRemoved.Handle);
	}
}
