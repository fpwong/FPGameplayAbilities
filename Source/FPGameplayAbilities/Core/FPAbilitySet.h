// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ActiveGameplayEffectHandle.h"
#include "FPGameplayAbilities/Public/AbilitySystem/FPGATypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "FPAbilitySet.generated.h"

class IAbilitySystemInterface;
class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPAbilitySetParameters
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FFPSetByCallerMagnitude> SetByCallerMagnitudes;
};

USTRUCT(BlueprintType)
struct FFPGrantedAbility
{
	GENERATED_BODY()

	UPROPERTY(BlueprintType, EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(BlueprintType, EditDefaultsOnly)
	int AbilityLevel;
};

USTRUCT(BlueprintType)
struct FFPGrantedGameplayEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintType, EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	UPROPERTY(BlueprintType, EditDefaultsOnly)
	int EffectLevel;
};

USTRUCT(BlueprintType)
struct FFPGrantedAttributeSet
{
	GENERATED_BODY()

	UPROPERTY(BlueprintType, EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPAbilitySetHandle
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	int HandleId = 0;

	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// UPROPERTY(Transient)
	// TArray<TWeakObjectPtr<UAttributeSet>> GrantedAttributeSets;

	bool IsValid();

	void Reset();

	void RemoveAbilitySet();
};

// Credits to Kaos Spectrum for the implementation of AbilitySets: https://www.thegames.dev/?p=89
USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPAbilitySet
{
	GENERATED_BODY()

public:
	const TArray<FFPGrantedAbility>& GetGrantedGameplayAbilities() const { return GrantedGameplayAbilities; }
	const TArray<FFPGrantedGameplayEffect>& GetGrantedGameplayEffects() const { return GrantedGameplayEffects; }

	FFPAbilitySetHandle GiveAbilitySetTo(UAbilitySystemComponent* ASC, UObject* OverrideSourceObject = nullptr) const;
	FFPAbilitySetHandle GiveAbilityWithParameters(UAbilitySystemComponent* ASC, const FFPAbilitySetParameters& Parameters, UObject* OverrideSourceObject = nullptr) const;
	void RemoveAbilitySet(FFPAbilitySetHandle& AbilitySetHandle);

protected:
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FFPGrantedAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FFPGrantedGameplayEffect> GrantedGameplayEffects;

	// // Attribute sets to grant when this ability set is granted.
	// UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	// TArray<FFPGrantedAttributeSet> GrantedAttributes;
};

UCLASS()
class FPGAMEPLAYABILITIES_API UFPAbilitySetLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = FPAbilitySet)
	static void RemoveAbilitySet(FFPAbilitySetHandle& AbilitySetHandle);

	UFUNCTION(BlueprintCallable, Category = FPAbilitySet)
	static FFPAbilitySetHandle GiveAbilitySet(const FFPAbilitySet& AbilitySet, UAbilitySystemComponent* AbilitySystem, UObject* OverrideSourceObject);
};