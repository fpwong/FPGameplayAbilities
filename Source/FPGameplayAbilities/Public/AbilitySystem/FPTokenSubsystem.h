// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Subsystems/EngineSubsystem.h"
#include "FPTokenSubsystem.generated.h"

class UAbilitySystemComponent;
struct FGameplayAttribute;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPTokenCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag TokenTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	int Cost = 1;
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPTokenCostList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	TArray<FFPTokenCost> Costs;

	bool CanAfford(UAbilitySystemComponent* ASC) const;
	bool SpendCosts(UAbilitySystemComponent* ASC) const;
};

// TODO test if this works for a networked game
UCLASS()
class FPGAMEPLAYABILITIES_API UFPTokenSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static UFPTokenSubsystem& Get();

	UFUNCTION(BlueprintCallable)
	UGameplayEffect* FindOrAddTokenEffect(FGameplayTag Tag, FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	int32 GetAttributeTokenCount(UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	void SpendAttributeToken(UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayAttribute Attribute, int Count = 1);

	UFUNCTION(BlueprintCallable)
	void GainAttributeToken(UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayAttribute Attribute, int Count = 1);

	UPROPERTY()
	TMap<int32, UGameplayEffect*> TokenMap;

	UFUNCTION(BlueprintCallable)
	static bool CanAffordCostList(const FFPTokenCostList& TokenCostList, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	static bool SpendCostList(const FFPTokenCostList& TokenCostList, UAbilitySystemComponent* ASC);
};
