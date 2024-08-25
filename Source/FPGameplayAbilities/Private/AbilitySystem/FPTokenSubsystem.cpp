// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPTokenSubsystem.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

bool FFPTokenCostList::CanAfford(UAbilitySystemComponent* ASC) const
{
	if (!ASC)
	{
		return false;
	}

	for (const FFPTokenCost& Cost : Costs)
	{
		// FFPTokenCost NewCost = EditCost(Cost);
		int32 CurrentAmount = UFPTokenSubsystem::Get().GetAttributeTokenCount(ASC, Cost.TokenTag, Cost.Attribute);

		if (Cost.Cost > CurrentAmount)
		{
			return false;
		}
	}

	return true;
}

bool FFPTokenCostList::SpendCosts(UAbilitySystemComponent* ASC) const
{
	if (!CanAfford(ASC))
	{
		return false;
	}

	for (const FFPTokenCost& Cost : Costs)
	{
		if (Cost.Cost >= 0)
		{
			UFPTokenSubsystem::Get().SpendAttributeToken(ASC, Cost.TokenTag, Cost.Attribute, Cost.Cost);
		}
		else // negative values give us tokens!
		{
			UFPTokenSubsystem::Get().GainAttributeToken(ASC, Cost.TokenTag, Cost.Attribute, FMath::Abs(Cost.Cost));
		}
	}

	return true;
}

UFPTokenSubsystem& UFPTokenSubsystem::Get()
{
	return *GEngine->GetEngineSubsystem<UFPTokenSubsystem>();
}

UGameplayEffect* UFPTokenSubsystem::FindOrAddTokenEffect(FGameplayTag Tag, FGameplayAttribute Attribute)
{
	const int32 Hash = HashCombine(GetTypeHash(Tag), GetTypeHash(Attribute));

	if (TokenMap.Contains(Hash))
	{
		return TokenMap[Hash];
	}

	FString Name = Tag.GetTagName().ToString() + "_" + Attribute.AttributeName;

	UGameplayEffect* TokenEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(Name));
	TokenEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;
	TokenEffect->StackingType = EGameplayEffectStackingType::AggregateByTarget;

	FGameplayModifierInfo ModInfo;
	ModInfo.Attribute = Attribute;
	ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.0)); 
	ModInfo.SourceTags.RequireTags.AddTag(Tag);
	TokenEffect->Modifiers.Add(ModInfo);

	UE_LOG(LogTemp, Display, TEXT("Added new GE %d %s"), Hash, *Name);

	TokenMap.Add(Hash, TokenEffect);
	return TokenEffect;
}

int32 UFPTokenSubsystem::GetAttributeTokenCount(UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayAttribute Attribute)
{
	return UFPGAGameplayAbilitiesLibrary::GetAttributeValueWithTags(ASC, Attribute, FGameplayTagContainer(Tag));
}

void UFPTokenSubsystem::SpendAttributeToken(UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayAttribute Attribute, int Count)
{
	if (ASC)
	{
		if (UGameplayEffect* TokenEffect = FindOrAddTokenEffect(Tag, Attribute))
		{
			UFPGAGameplayAbilitiesLibrary::RemoveGameplayEffect(TokenEffect, ASC, Count);
		}
	}
}

void UFPTokenSubsystem::GainAttributeToken(UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayAttribute Attribute, int Count)
{
	if (ASC)
	{
		if (UGameplayEffect* TokenEffect = FindOrAddTokenEffect(Tag, Attribute))
		{
			UFPGAGameplayAbilitiesLibrary::ApplyGameplayEffect(TokenEffect, ASC, ASC, 1, FGameplayEffectContextHandle(), Count);
		}
	}
}

bool UFPTokenSubsystem::CanAffordCostList(const FFPTokenCostList& TokenCostList, UAbilitySystemComponent* ASC)
{
	return TokenCostList.CanAfford(ASC);
}

bool UFPTokenSubsystem::SpendCostList(const FFPTokenCostList& TokenCostList, UAbilitySystemComponent* ASC)
{
	return TokenCostList.SpendCosts(ASC);
}
