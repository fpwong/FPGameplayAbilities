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
	LessEquals,
	GreaterEquals,
};

UENUM()
enum class EFPAttributeComparison_ValueType : uint8
{
	StaticValue,
	AttributeValue,
};

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API UFPTargetFilterTask_AttributeComparison final : public UFPTargetFilterTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Default)
	EFPAttributeComparison_ComparisonMethod ComparisonMethod = EFPAttributeComparison_ComparisonMethod::Equals;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	FGameplayAttribute Attribute;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	EFPAttributeComparison_ValueType ValueType = EFPAttributeComparison_ValueType::StaticValue;

	UPROPERTY(EditDefaultsOnly, Category=Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::AttributeValue", EditConditionHides))
	FGameplayEffectModifierMagnitude AttributeValue;

	UPROPERTY(EditDefaultsOnly, Category = Default, meta = (EditCondition = "ValueType == EFPAttributeComparison_ValueType::StaticValue", EditConditionHides))
	float StaticValue = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = Default)
	bool bInvert = false;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const override;

	float GetValue() const;

	virtual FFPTargetFilterObserver* MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;
};

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterObserver_AttributeComparison : public FFPTargetFilterObserver
{
	GENERATED_BODY()

	virtual ~FFPTargetFilterObserver_AttributeComparison() override;

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

	void InitAttributeValue(UFPTargetFilterTask_AttributeComparison* FilterTask, AActor* SourceActor, AActor* TargetActor);

	void OnAttributeChanged(const FOnAttributeChangeData& Data);

	FGameplayAttribute Attribute;

private:
	FDelegateHandle AttributeChangedHandle;
};
