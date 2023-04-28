// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"


struct FAttributeDefaultValueList
{
	void AddPair(FProperty* InProperty, float InValue)
	{
		// List.Add(FOffsetValuePair(InProperty, InValue));
		AttributeValues.FindOrAdd(InProperty, InValue);
	}

	struct FOffsetValuePair
	{
		FOffsetValuePair(FProperty* InProperty, float InValue)
		: Property(InProperty), Value(InValue) { }

		FProperty* Property;
		float Value;
	};

	TMap<FProperty*, float> AttributeValues;
};

struct FAttributeSetDefaults
{
	TMap<TSubclassOf<UAttributeSet>, FAttributeDefaultValueList> DataMap;
};

struct FAttributeSetDefaultsCollection
{
	TArray<FAttributeSetDefaults>		LevelData;

	float GetValue(UAttributeSet* AttributeSet, FName GroupName, int Level);
};

/* The only purpose of this is to replace FindBestAttributeClass and not check name using FString::Contains */
struct FPGAMEPLAYABILITIES_API FFPGAAttributeSetInitter : public FAttributeSetInitter
{
public:
	virtual void PreloadAttributeSetData(const TArray<UCurveTable*>& CurveData) override;
	virtual void InitAttributeSetDefaults(UAbilitySystemComponent* AbilitySystemComponent, FName GroupName, int32 Level, bool bInitialInit) const override;
	virtual void ApplyAttributeDefault(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, FName GroupName, int32 Level) const override;
	virtual TArray<float> GetAttributeSetValues(UClass* AttributeSetClass, FProperty* AttributeProperty, FName GroupName) const override;

	virtual void InitAttributeSetValues(UAttributeSet* AttributeSet, const FName& GroupName, int32 Level, bool bInitialInit);

	const FAttributeDefaultValueList* GetDefaultValueList(UAttributeSet* AttributeSet, const FName& GroupName, int32 Level, bool bInitialInit);

protected:
	bool IsSupportedProperty(FProperty* Property) const;

	TSubclassOf<UAttributeSet> FindBestAttributeClass(TArray<TSubclassOf<UAttributeSet>>& ClassList, FString PartialName);

	TMap<FName, FAttributeSetDefaultsCollection> Defaults;
};

