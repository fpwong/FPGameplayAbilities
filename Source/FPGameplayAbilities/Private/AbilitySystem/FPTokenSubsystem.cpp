// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPTokenSubsystem.h"

#include "GameplayEffect.h"

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
