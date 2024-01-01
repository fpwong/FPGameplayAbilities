// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTask.h"
#include "FPTargetFilterObserver.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

#include "FPTargetFilterTask_AttributeComparison.generated.h"

UENUM()
enum class EFPAttributeComparison_ComparisonMethod : uint8
{
	Equals,
	Less,
	LessEquals,
	Greater,
	GreaterEquals
};

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

	UPROPERTY(EditDefaultsOnly, Category = Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::NumericValue", EditConditionHides))
	float NumericValue = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::AttributeValue", EditConditionHides))
	FFPAttributeComparison_AttributeValue AttributeValue;

	UPROPERTY(EditDefaultsOnly, Category = Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::AttributeValue", EditConditionHides))
	EFPTargetFilterTaskContext Context = EFPTargetFilterTaskContext::Target;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	EFPAttributeComparison_ValueType ValueType = EFPAttributeComparison_ValueType::AttributeValue;

	float GetValue(UAbilitySystemComponent* AbilitySystem) const;

	bool IsValid(UAbilitySystemComponent* AbilitySystem) const;
};

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API UFPTargetFilterTask_AttributeComparison final : public UFPTargetFilterTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Default)
	EFPAttributeComparison_ComparisonMethod ComparisonMethod = EFPAttributeComparison_ComparisonMethod::Equals;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FFPAttributeComparison_Context ContextA;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FFPAttributeComparison_Context ContextB;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	bool bInvert = false;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override;

	virtual FFPTargetFilterObserver* MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;
};

USTRUCT()
struct FFPTargetFilterObserverBinding
{
	GENERATED_BODY()

	FDelegateHandle Handle;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

	FGameplayAttribute Attribute;

	void Reset()
	{
		Handle.Reset();
		AbilitySystem.Reset();
	}
};

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterObserver_AttributeComparison : public FFPTargetFilterObserver
{
	GENERATED_BODY()

	virtual ~FFPTargetFilterObserver_AttributeComparison() override;

	void InitAttributeValue(UFPTargetFilterTask_AttributeComparison* FilterTask, AActor* SourceActor, AActor* TargetActor);

	void OnAttributeChanged(const FOnAttributeChangeData& Data);

private:
	FFPTargetFilterObserverBinding BindingA;
	FFPTargetFilterObserverBinding BindingB;
};
