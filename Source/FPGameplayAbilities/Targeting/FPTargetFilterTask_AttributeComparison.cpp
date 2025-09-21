// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterTask_AttributeComparison.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"

float FFPAttributeComparison_AttributeValue::GetValue(UAbilitySystemComponent* AbilitySystem) const
{
	return (AbilitySystem->GetNumericAttribute(Attribute) + PreMultiplyAdditiveValue.GetValue() * Coefficient.GetValue()) + PostMultiplyAdditiveValue.GetValue();
}

float FFPAttributeComparison_Context::GetValue(UAbilitySystemComponent* AbilitySystem) const
{
	if (ValueType == EFPAttributeComparison_ValueType::NumericValue)
	{
		return NumericValue;
	}

	return AttributeValue.GetValue(AbilitySystem);
}

bool FFPAttributeComparison_Context::IsValid(UAbilitySystemComponent* AbilitySystem) const
{
	if (ValueType == EFPAttributeComparison_ValueType::AttributeValue)
	{
		return AbilitySystem != nullptr;
	}

	return true;
}

bool FFPTargetFilterTask_AttributeComparison::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags) const
{
	if (!TargetActor)
	{
		return true;
	}

	UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	UAbilitySystemComponent* SourceAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor);

	UAbilitySystemComponent* AbilitySystemA = ContextA.Context == EFPTargetFilterTaskContext::Source ? SourceAbilitySystem : TargetAbilitySystem;
	UAbilitySystemComponent* AbilitySystemB = ContextB.Context == EFPTargetFilterTaskContext::Source ? SourceAbilitySystem : TargetAbilitySystem;

	if (!ContextA.IsValid(AbilitySystemA) || !ContextB.IsValid(AbilitySystemB))
	{
		return false;
	}

	const float ValueA = ContextA.GetValue(AbilitySystemA);
	const float ValueB = ContextB.GetValue(AbilitySystemB);

	// UE_LOG(LogTemp, Warning, TEXT("Value %f %f"), ValueA, ValueB);

	bool bResult = false;
	switch (ComparisonMethod)
	{
	case EFPAttributeComparison_ComparisonMethod::Equals:
		bResult = ValueA == ValueB;
		break;
	case EFPAttributeComparison_ComparisonMethod::Less:
		bResult = ValueA < ValueB;
		break;
	case EFPAttributeComparison_ComparisonMethod::LessEquals:
		bResult = ValueA <= ValueB;
		break;
	case EFPAttributeComparison_ComparisonMethod::Greater:
		bResult = ValueA > ValueB;
		break;
	case EFPAttributeComparison_ComparisonMethod::GreaterEquals:
		bResult = ValueA >= ValueB;
		break;
	default: ;
	}

	return bResult ^ bInvert;
}

void FFPTargetFilterTask_AttributeComparison::BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const
{
	UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	UAbilitySystemComponent* SourceAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor);

	UAbilitySystemComponent* SysA = ContextA.Context == EFPTargetFilterTaskContext::Source ? SourceAbilitySystem : TargetAbilitySystem;
	if (SysA && ContextA.ValueType == EFPAttributeComparison_ValueType::AttributeValue)
	{
		BindToAbilitySystem(Observer, SysA);
	}

	UAbilitySystemComponent* SysB = ContextB.Context == EFPTargetFilterTaskContext::Source ? SourceAbilitySystem : TargetAbilitySystem;
	if (SysB && ContextB.ValueType == EFPAttributeComparison_ValueType::AttributeValue)
	{
		BindToAbilitySystem(Observer, SysB);
	}
}

void FFPTargetFilterTask_AttributeComparison::BindToAbilitySystem(FFPTargetFilterObserver& Observer, UAbilitySystemComponent* Sys) const
{
	FGameplayAttribute Attrib = ContextA.AttributeValue.Attribute;
	FDelegateHandle Handle = Sys->GetGameplayAttributeValueChangeDelegate(Attrib).AddLambda([&Observer](const FOnAttributeChangeData& Data)
	{
		Observer.CheckResultChanged();
	});

	Observer.AddCleanup([Handle, Sys = TWeakObjectPtr(Sys), Attrib]
	{
		if (Sys.IsValid())
		{
			Sys->GetGameplayAttributeValueChangeDelegate(Attrib).Remove(Handle);
		}
	});
}
