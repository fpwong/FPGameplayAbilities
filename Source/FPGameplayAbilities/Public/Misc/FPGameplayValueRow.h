// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FPGameplayValueRow.generated.h"

struct FGameplayTagContainer;
class UAbilitySystemComponent;
struct FGameplayTag;

UCLASS(BlueprintType, Abstract)
class FPGAMEPLAYABILITIES_API UFPValueCalculation : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual float Calculate(UAbilitySystemComponent* ASC, float BaseValue, const FGameplayTagContainer& Tags) { return BaseValue; }
};

UCLASS(BlueprintType, Abstract)
class FPGAMEPLAYABILITIES_API UFPValueDisplay : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FString GetDisplayString(float Value) { return FString::SanitizeFloat(Value, 0); }
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGameplayValueRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UFPValueCalculation> ValueCalculation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UFPValueDisplay> ValueDisplayMethod;
};

UCLASS(BlueprintType, DefaultToInstanced)
class FPGAMEPLAYABILITIES_API UFPGameplayValueHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool GetBaseValueFromTable(UDataTable* DataTable, FGameplayTag Tag, float& Value);

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool GetTransformedValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayTagContainer EffectTags, float& OutTransformedValue);

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool GetDisplayValueFromTable(UDataTable* DataTable, FGameplayTag Tag, FString& OutString);
};