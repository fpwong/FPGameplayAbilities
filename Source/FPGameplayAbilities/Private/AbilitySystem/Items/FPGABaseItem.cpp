// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Items/FPGABaseItem.h"
#include "AbilitySystemComponent.h"

FString UFPGABaseItem::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

FPrimaryAssetId UFPGABaseItem::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix
	return FPrimaryAssetId(ItemType, GetFName());
}

void UFPGABaseItem::ApplyItem(UAbilitySystemComponent* AbilitySystem)
{
	if (!AbilitySystem)
		return;

	for (TSubclassOf<UGameplayAbility> Ability : GrantedAbilities)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Granted ability %s"), *Ability->GetName());
		FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(Ability.GetDefaultObject()));
		AbilitySpecHandles.Add(AbilitySpecHandle);
	}

	for (TSubclassOf<UGameplayEffect> Effect : GrantedEffects)
	{
		AbilitySystem->BP_ApplyGameplayEffectToSelf(Effect, 0.f, FGameplayEffectContextHandle());
	}
}

void UFPGABaseItem::RemoveItem(UAbilitySystemComponent* AbilitySystem)
{
	if (!AbilitySystem)
		return;

	for (FGameplayAbilitySpecHandle Ability : AbilitySpecHandles)
	{
		AbilitySystem->ClearAbility(Ability);
	}

	for (TSubclassOf<UGameplayEffect> Effect : GrantedEffects)
	{
		AbilitySystem->RemoveActiveGameplayEffectBySourceEffect(Effect, nullptr, 1);
	}
}
