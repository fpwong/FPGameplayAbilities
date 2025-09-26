// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FPGameplayValueRow.generated.h"

class UGameplayEffect;
class UGameplayAbility;
struct FGameplayEffectSpecHandle;
struct FGameplayTagContainer;
class UAbilitySystemComponent;
struct FGameplayTag;

UCLASS(BlueprintType, Blueprintable, Abstract)
class FPGAMEPLAYABILITIES_API UFPValueCalculation : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual float Calculate(UAbilitySystemComponent* ASC, float BaseValue, const FGameplayTagContainer& Tags) { return BaseValue; }
};

UCLASS(BlueprintType, Blueprintable, Abstract)
class FPGAMEPLAYABILITIES_API UFPValueDisplay : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FString GetDisplayString(float Value) { return FString::SanitizeFloat(Value, 0); }
};

UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="Display Percent"))
class FPGAMEPLAYABILITIES_API UFPValueDisplay_Percent : public UFPValueDisplay
{
	GENERATED_BODY()

public:
	virtual FString GetDisplayString(float Value) override { return FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Value) * 100); }
};

UCLASS(BlueprintType)
class FPGAMEPLAYABILITIES_API UFPValueSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UFPValueCalculation> ValueCalculation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUseCalculationOnlyForDisplayValue = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UFPValueDisplay> ValueDisplayMethod;
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGameplayValueRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ValueName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UFPValueSettings> Settings;

	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// TSubclassOf<UFPValueCalculation> ValueCalculation;
	//
	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// TSubclassOf<UFPValueDisplay> ValueDisplayMethod;
	//
	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// bool bValueCalculationOnlyForDisplayValue = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayTag> Tags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayTag> ScalingTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCurveTableRowHandle ScalingCurve;

	float GetValueAtLevel(int Level = -1) const;
	float GetBaseValue(UAbilitySystemComponent* ASC, int Level = 1) const;
	int GetScalingLevel(UAbilitySystemComponent* ASC) const;
};

UCLASS(BlueprintType)
class FPGAMEPLAYABILITIES_API UFPGameplayValueHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool GetBaseValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, float& Value, int Level = 1);

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool GetTransformedValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayTagContainer EffectTags, float& OutTransformedValue, int Level = 1);

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool GetDisplayValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, FString& OutString, int Level = 1);

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool GetTransformedDisplayValueFromTable(UDataTable* DataTable, FGameplayTag Tag, UAbilitySystemComponent* ASC, FGameplayTagContainer EffectTags, FString& OutString, FFPGameplayValueRow& OutRow, int Level = 1);

	UFUNCTION(BlueprintCallable)
	static void ApplyGameValueTableToSpec(UAbilitySystemComponent* ASC, FGameplayEffectSpecHandle SpecHandle, UDataTable* DataTable);

	UFUNCTION(BlueprintCallable)
	static FGameplayTagContainer GatherTagsFromGameplayAbility(const UGameplayAbility* GameplayAbility, FGameplayTag GameValueTag, UDataTable* DataTable);

	UFUNCTION(BlueprintCallable)
	static FString ApplyValueDisplayMethod(TSubclassOf<UFPValueDisplay> ValueDisplayMethod, float Value);

	UFUNCTION(BlueprintCallable)
	static bool DoesGameValueTableHaveTag(FGameplayTag GameValueTag, UDataTable* DataTable);

	// helper functions to calculate the level of a row in the table 
	static FGameplayTag TransformScalingTag(FGameplayTag BaseTag);
	// static FGameplayTag GetScalingTagFromRow(const FFPGameplayValueRow* Row);
	// static int GetScalingLevelForRow(UAbilitySystemComponent* ASC, const FFPGameplayValueRow* Row);

	static FGameplayEffectSpecHandle MakeGameplayEffectSpecFromTable(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> GameplayEffectClass, int Level = 1);
};
