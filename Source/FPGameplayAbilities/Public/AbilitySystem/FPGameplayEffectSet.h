// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FPGameplayEffectSet.generated.h"

struct FGameplayEffectSpecHandle;
class UAbilitySystemComponent;
struct FActiveGameplayEffectHandle;

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGameplayEffectSetHandle
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	int HandleId = 0;

	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	bool IsValid() const;

	void Reset();

	void RemoveSet();
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGameplayEffectSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayEffectSpecHandle> EffectSpecs;

public:
	FFPGameplayEffectSetHandle ApplyGameplayEffectSet(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC) const;
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGameplayEffectSetList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FFPGameplayEffectSetHandle> ActiveSets;

	FFPGameplayEffectSetHandle ApplyEffectSetToActor(const FFPGameplayEffectSet& EffectSet, UAbilitySystemComponent* SourceASC, AActor* Actor);
	bool RemoveActor(AActor* Actor);
};

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGameplayEffectSetLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = FPAbilitySet)
	static FFPGameplayEffectSet MakeGameplayEffectSetFromClass(UAbilitySystemComponent* ASC, TArray<FFPGrantedGameplayEffect> GameplayEffects);

	UFUNCTION(BlueprintCallable, Category = FPAbilitySet)
	static FFPGameplayEffectSetHandle ApplyGameplayEffectSet(const FFPGameplayEffectSet& AbilitySet, UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC);

	UFUNCTION(BlueprintCallable, Category = FPAbilitySet)
	static FFPGameplayEffectSetHandle AddToGameplayEffectSetList(UPARAM(ref) FFPGameplayEffectSetList& List, const FFPGameplayEffectSet& EffectSet, UAbilitySystemComponent* SourceASC, AActor* Actor)
	{
		return List.ApplyEffectSetToActor(EffectSet, SourceASC, Actor);
	}

	UFUNCTION(BlueprintCallable, Category = FPAbilitySet)
	static bool RemoveFromGameplayEffectSetList(UPARAM(ref) FFPGameplayEffectSetList& List, AActor* Actor)
	{
		return List.RemoveActor(Actor);
	}

	UFUNCTION(BlueprintCallable, Category = FPAbilitySet)
	static void RemoveGameplayEffectSet(UPARAM(ref) FFPGameplayEffectSetHandle& EffectSetHandle);

	UFUNCTION(BlueprintCallable, Category = FPAbilitySet, meta=(DisplayName="IsValid"))
	static bool IsAbilitySetHandleValid(const FFPGameplayEffectSetHandle& Handle) { return Handle.IsValid(); }
};
