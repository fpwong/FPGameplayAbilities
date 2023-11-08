// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGameplayEffectSet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FPGameplayAbilities/Core/FPAbilitySet.h"

namespace FPGameplayEffectSetHandle_Impl
{
	static int32 LastHandleId = 0;
	static int32 GetNextQueuedHandleIdForUse() { return ++LastHandleId; }
}

bool FFPGameplayEffectSetHandle::IsValid() const
{
	return HandleId > 0;
}

void FFPGameplayEffectSetHandle::Reset() {}

void FFPGameplayEffectSetHandle::RemoveSet()
{
	if (AbilitySystemComponent.IsValid())
	{
		for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
		}
	}
}

FFPGameplayEffectSetHandle FFPGameplayEffectSet::ApplyGameplayEffectSet(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC) const
{
	if (!TargetASC || !TargetASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return FFPGameplayEffectSetHandle();
	}

	if (EffectSpecs.Num() == 0)
	{
		return FFPGameplayEffectSetHandle();
	}

	FFPGameplayEffectSetHandle OutHandle;
	OutHandle.HandleId = FPGameplayEffectSetHandle_Impl::GetNextQueuedHandleIdForUse();
	OutHandle.AbilitySystemComponent = TargetASC;

	for (FGameplayEffectSpecHandle EffectSpec : EffectSpecs)
	{
		FActiveGameplayEffectHandle GameplayEffectHandle; 
		if (SourceASC)
		{
			GameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(), TargetASC);
		}
		else
		{
			GameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
		}

		if (GameplayEffectHandle.IsValid())
		{
			OutHandle.GameplayEffectHandles.Add(GameplayEffectHandle);
		}
	}

	return OutHandle;
}

FFPGameplayEffectSetHandle FFPGameplayEffectSetList::ApplyEffectSetToActor(const FFPGameplayEffectSet& EffectSet, UAbilitySystemComponent* SourceASC, AActor* Actor)
{
	FFPGameplayEffectSetHandle Handle = EffectSet.ApplyGameplayEffectSet(SourceASC, UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor));
	if (Handle.IsValid())
	{
		ActiveSets.Add(Actor, Handle);
	}

	return Handle;
}

bool FFPGameplayEffectSetList::RemoveActor(AActor* Actor)
{
	if (FFPGameplayEffectSetHandle* EffectSetHandle = ActiveSets.Find(Actor))
	{
		EffectSetHandle->RemoveSet();
		return true;
	}

	return false;
}

FFPGameplayEffectSet UFPGameplayEffectSetLibrary::MakeGameplayEffectSetFromClass(UAbilitySystemComponent* ASC, TArray<FFPGrantedGameplayEffect> GameplayEffects)
{
	if (!ASC)
	{
		return FFPGameplayEffectSet();
	}

	FFPGameplayEffectSet GESet;
	Algo::Transform(GameplayEffects, GESet.EffectSpecs, [ASC](const FFPGrantedGameplayEffect& GrantedGE)
	{
		return ASC->MakeOutgoingSpec(GrantedGE.GameplayEffect, GrantedGE.EffectLevel, ASC->MakeEffectContext());
	});

	return GESet;
}

FFPGameplayEffectSetHandle UFPGameplayEffectSetLibrary::ApplyGameplayEffectSet(const FFPGameplayEffectSet& AbilitySet, UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC)
{
	return AbilitySet.ApplyGameplayEffectSet(SourceASC, TargetASC);
}

void UFPGameplayEffectSetLibrary::RemoveGameplayEffectSet(FFPGameplayEffectSetHandle& EffectSetHandle)
{
	EffectSetHandle.RemoveSet();
}
