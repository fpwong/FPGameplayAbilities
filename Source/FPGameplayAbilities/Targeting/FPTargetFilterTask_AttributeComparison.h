// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FPTargetFilterObserver.h"
#include "GameplayEffect.h"

#include "FPTargetFilterTask_AttributeComparison.generated.h"

UENUM()
enum class EFPAttributeComparison_ValueType : uint8
{
	NumericValue,
	AttributeValue,
};

USTRUCT(meta=(ShowOnlyInnerProperties))
struct FFPAttributeComparison_AttributeValue
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FGameplayAttribute Attribute;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FScalableFloat Coefficient;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FScalableFloat PreMultiplyAdditiveValue;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FScalableFloat PostMultiplyAdditiveValue;

	float GetValue(UAbilitySystemComponent* AbilitySystem) const;
};

USTRUCT(meta=(ShowOnlyInnerProperties))
struct FFPAttributeComparison_Context
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Default)
	EFPAttributeComparison_ValueType ValueType = EFPAttributeComparison_ValueType::AttributeValue;

	UPROPERTY(EditDefaultsOnly, Category = Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::NumericValue", EditConditionHides))
	float NumericValue = 0.0f; // Perhaps this could be a scalable float?

	UPROPERTY(EditDefaultsOnly, Category = Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::AttributeValue", EditConditionHides))
	EFPTargetFilterTaskContext Context = EFPTargetFilterTaskContext::Target;

	UPROPERTY(EditDefaultsOnly, Category = Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::AttributeValue", EditConditionHides))
	FFPAttributeComparison_AttributeValue AttributeValue;

	float GetValue(UAbilitySystemComponent* AbilitySystem) const;

	bool IsValid(UAbilitySystemComponent* AbilitySystem) const;
};

USTRUCT(BlueprintType, meta=(DisplayName="TargetFilterTask Attribute Comparison"))
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTask_AttributeComparison : public FFPTargetFilterTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Default)
	EFPNumericComparisonMethod ComparisonMethod = EFPNumericComparisonMethod::Equals;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FFPAttributeComparison_Context ContextA;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FFPAttributeComparison_Context ContextB;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	bool bInvert = false;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override;

	virtual void BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const override;

	virtual void BindToAbilitySystem(FFPTargetFilterObserver& Observer, UAbilitySystemComponent* Sys) const;
};
