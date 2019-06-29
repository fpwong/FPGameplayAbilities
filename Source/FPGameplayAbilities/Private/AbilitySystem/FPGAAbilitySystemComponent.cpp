// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAAbilitySystemComponent.h"

void UFPGAAbilitySystemComponent::BeginPlay()
{
	UAbilitySystemComponent::BeginPlay();

	OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UFPGAAbilitySystemComponent::OnGEAppliedToSelf);

	OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UFPGAAbilitySystemComponent::OnGERemoved);

	//RegisterGameplayTagEvent(, EGameplayTagEventType::NewOrRemoved)
}

FActiveGameplayEffectHandle UFPGAAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey)
{
	FActiveGameplayEffectHandle MyHandle = UAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(Spec, PredictionKey);

	if (MyHandle == FActiveGameplayEffectHandle())
		return MyHandle;

	const bool bIsNetAuthority = IsOwnerActorAuthoritative();

	if (!bIsNetAuthority && Spec.Def->RemoveGameplayEffectsWithTags.CombinedTags.Num() > 0)
	{
		// Clear tags is always removing all stacks.
		FGameplayEffectQuery ClearQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(Spec.Def->RemoveGameplayEffectsWithTags.CombinedTags);
		if (MyHandle.IsValid())
		{
			ClearQuery.IgnoreHandles.Add(MyHandle);
		}
		ActiveGameplayEffects.RemoveActiveEffects(ClearQuery, -1);
	}

	return MyHandle;
}

void UFPGAAbilitySystemComponent::LocalInputConfirm()
{
	GenericLocalConfirmCallbacks.Broadcast();
}

void UFPGAAbilitySystemComponent::LocalInputCancel()
{
	GenericLocalCancelCallbacks.Broadcast();
	OnInputCancel.Broadcast();
}

void UFPGAAbilitySystemComponent::OnGEAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	OnGameplayEffectAppliedToSelf.Broadcast(ActiveHandle);
}

void UFPGAAbilitySystemComponent::OnGERemoved(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	OnGameplayEffectRemoved.Broadcast(ActiveGameplayEffect.Handle);
}

void UFPGAAbilitySystemComponent::BindToAttributeChanged(FGameplayAttribute Attribute, FFPGAAttributeChangedDelegate OnAttributeChanged)
{
	BP_AttributeValueChangeDelegates.Add(Attribute, OnAttributeChanged);
	GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UFPGAAbilitySystemComponent::ExecuteOnAttributeChanged);
}

void UFPGAAbilitySystemComponent::K2_RegisterGameplayTagEvent(FGameplayTag EventTag)
{
	FOnGameplayEffectTagCountChanged& CountChangedEvent = RegisterGameplayTagEvent(EventTag);
	//CountChangedEvent.AddRaw();
}

void UFPGAAbilitySystemComponent::ExecuteOnAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	if (BP_AttributeValueChangeDelegates.Contains(ChangeData.Attribute))
	{
		FFPGAAttributeChangedDelegate Delegate = BP_AttributeValueChangeDelegates[ChangeData.Attribute];
		if (Delegate.IsBound())
		{
			Delegate.Execute(ChangeData.OldValue, ChangeData.NewValue);
		}
	}
}
