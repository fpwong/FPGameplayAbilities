// Copyright 2020 Dan Kestranek.

#include "AbilitySystem/Tasks/FPGAAsyncTask_EffectStackChanged.h"

UFPGAAsyncTask_EffectStackChanged * UFPGAAsyncTask_EffectStackChanged::ListenForGameplayEffectStackChange(FActiveGameplayEffectHandle Handle)
{
	if (!Handle.IsValid() || !Handle.GetOwningAbilitySystemComponent())
	{
		return nullptr;
	}

	UFPGAAsyncTask_EffectStackChanged* ListenForGameplayEffectStackChange = NewObject<UFPGAAsyncTask_EffectStackChanged>();
	ListenForGameplayEffectStackChange->ActiveHandle = Handle;

	if (FOnActiveGameplayEffectStackChange* StackChangeDelegate = Handle.GetOwningAbilitySystemComponent()->OnGameplayEffectStackChangeDelegate(Handle))
	{
		StackChangeDelegate->AddUObject(ListenForGameplayEffectStackChange, &UFPGAAsyncTask_EffectStackChanged::GameplayEffectStackChanged);
	}

	return ListenForGameplayEffectStackChange;
}

void UFPGAAsyncTask_EffectStackChanged::EndTask()
{
	if (UAbilitySystemComponent* ASC = ActiveHandle.GetOwningAbilitySystemComponent())
	{
		if (FOnActiveGameplayEffectStackChange* StackChangeDelegate = ASC->OnGameplayEffectStackChangeDelegate(ActiveHandle))
		{
			StackChangeDelegate->RemoveAll(this);
		}
	}

	SetReadyToDestroy();
}

void UFPGAAsyncTask_EffectStackChanged::GameplayEffectStackChanged(FActiveGameplayEffectHandle EffectHandle, int32 NewStackCount, int32 PreviousStackCount)
{
	OnGameplayEffectStackChange.Broadcast(EffectHandle, NewStackCount, PreviousStackCount);
}
