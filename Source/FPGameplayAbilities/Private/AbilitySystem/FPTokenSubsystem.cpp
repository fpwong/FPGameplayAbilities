// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPTokenSubsystem.h"

#include "GameplayEffect.h"

UFPTokenSubsystem& UFPTokenSubsystem::Get()
{
	return *GEngine->GetEngineSubsystem<UFPTokenSubsystem>();
}

UGameplayEffect* UFPTokenSubsystem::FindOrAddTokenEffect(FGameplayTag Tag, FGameplayAttribute Attribute)
{
	if (TokenMap.Contains(Tag))
	{
		return TokenMap[Tag];
	}

	FString Name = Tag.GetTagName().ToString() + "_Token";

	UGameplayEffect* TokenEffect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(Name));
	TokenEffect->DurationPolicy = EGameplayEffectDurationType::Infinite;
	TokenEffect->StackingType = EGameplayEffectStackingType::AggregateByTarget;

	FGameplayModifierInfo ModInfo;
	ModInfo.Attribute = Attribute;
	ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.0)); 
	ModInfo.SourceTags.RequireTags.AddTag(Tag);
	TokenEffect->Modifiers.Add(ModInfo);

	TokenMap.Add(Tag, TokenEffect);
	return TokenEffect;
}
