// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Effects/FPTargetFilterGEComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

bool UFPTargetFilterGEComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	AActor* TargetActor = ActiveGEContainer.Owner->GetOwnerActor();
	AActor* SourceActor = GESpec.GetEffectContext().GetInstigator();
	// UE_LOG(LogTemp, Warning, TEXT("%hs - Src %s Tar %s"), __FUNCTION__, *GetNameSafe(SourceActor), *GetNameSafe(TargetActor));
	return TargetFilter.DoesFilterPass(SourceActor, TargetActor);
}
