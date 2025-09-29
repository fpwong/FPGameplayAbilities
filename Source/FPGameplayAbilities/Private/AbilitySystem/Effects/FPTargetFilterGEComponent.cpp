// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Effects/FPTargetFilterGEComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilterObserver.h"

bool UFPTargetFilterGEComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	if (bInhibits)
	{
		return true;
	}

	if (bBlocksApplication)
	{
		AActor* TargetActor = ActiveGEContainer.Owner->GetOwnerActor();
		AActor* SourceActor = GESpec.GetEffectContext().GetInstigator();
		return TargetFilter.DoesFilterPass(SourceActor, TargetActor);
	}

	return Super::CanGameplayEffectApply(ActiveGEContainer, GESpec);
}

bool UFPTargetFilterGEComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	if (bInhibits)
	{
		UAbilitySystemComponent* ASC = ActiveGEContainer.Owner;
		if (!ensure(ASC))
		{
			return false;
		}

		AActor* TargetActor = ActiveGEContainer.Owner->GetOwnerActor();
		AActor* SourceActor = ActiveGE.Spec.GetEffectContext().GetInstigator();

		FActiveGameplayEffectHandle ActiveGEHandle = ActiveGE.Handle;
		if (FActiveGameplayEffectEvents* EventSet = ASC->GetActiveEffectEventSet(ActiveGEHandle))
		{
			TSharedPtr<FFPTargetFilterObserver> Observer = MakeShared<FFPTargetFilterObserver>();
			Observer->OnResultChanged.AddUObject(this, &UFPTargetFilterGEComponent::OnResultChanged, ActiveGEHandle);
			Observer->Init(TargetFilter, SourceActor, TargetActor);

			EventSet->OnEffectRemoved.AddUObject(this, &UFPTargetFilterGEComponent::OnGameplayEffectRemoved, ASC, Observer);
		}

		return TargetFilter.DoesFilterPass(SourceActor, TargetActor);
	}

	return Super::OnActiveGameplayEffectAdded(ActiveGEContainer, ActiveGE);
}

void UFPTargetFilterGEComponent::OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& GERemovalInfo, UAbilitySystemComponent* ASC, TSharedPtr<FFPTargetFilterObserver> Observer) const
{
	if (Observer.IsValid())
	{
		Observer->Clear();
	}
}

void UFPTargetFilterGEComponent::OnResultChanged(AActor* Actor, bool bFilterPassed, FActiveGameplayEffectHandle ActiveGEHandle) const
{
	UAbilitySystemComponent* Owner = ActiveGEHandle.GetOwningAbilitySystemComponent();
	if (!Owner)
	{
		return;
	}

	if (bRemoveOnFail)
	{
		Owner->RemoveActiveGameplayEffect(ActiveGEHandle);
	}
	else if (bInhibits)
	{
		// It's possible for this to return nullptr if it was in the process of being removed (IsPendingRemove)
		const FActiveGameplayEffect* ActiveGE = Owner->GetActiveGameplayEffect(ActiveGEHandle);
		if (ActiveGE)
		{
			constexpr bool bInvokeCuesIfStateChanged = true;
			Owner->SetActiveGameplayEffectInhibit(MoveTemp(ActiveGEHandle), !bFilterPassed, bInvokeCuesIfStateChanged);
		}
	}
}