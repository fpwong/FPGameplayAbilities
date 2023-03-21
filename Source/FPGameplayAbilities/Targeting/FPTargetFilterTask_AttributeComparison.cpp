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

bool UFPTargetFilterTask_AttributeComparison::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags) const
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
		case EFPAttributeComparison_ComparisonMethod::LessEquals:
			bResult = ValueA <= ValueB;
			break;
		case EFPAttributeComparison_ComparisonMethod::GreaterEquals:
			bResult = ValueA >= ValueB;
			break;
		default: ;
	}

	return bResult ^ bInvert;
}

FFPTargetFilterObserver* UFPTargetFilterTask_AttributeComparison::MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	FFPTargetFilterObserver_AttributeComparison* Binding = new FFPTargetFilterObserver_AttributeComparison();
	Binding->InitAttributeValue(Cast<UFPTargetFilterTask_AttributeComparison>(FilterTask), SourceActor, TargetActor);
	return Binding;
}

FFPTargetFilterObserver_AttributeComparison::~FFPTargetFilterObserver_AttributeComparison()
{
	if (BindingA.AbilitySystem.IsValid())
	{
		BindingA.AbilitySystem->GetGameplayAttributeValueChangeDelegate(BindingA.Attribute).Remove(BindingA.Handle);
	}

	if (BindingB.AbilitySystem.IsValid())
	{
		BindingB.AbilitySystem->GetGameplayAttributeValueChangeDelegate(BindingB.Attribute).Remove(BindingB.Handle);
	}
}

void FFPTargetFilterObserver_AttributeComparison::InitAttributeValue(UFPTargetFilterTask_AttributeComparison* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	Init(FilterTask, SourceActor, TargetActor);

	BindingA.Reset();
	BindingB.Reset();

	UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	UAbilitySystemComponent* SourceAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor);

	UAbilitySystemComponent* AbilitySystemA = FilterTask->ContextA.Context == EFPTargetFilterTaskContext::Source ? SourceAbilitySystem : TargetAbilitySystem;
	UAbilitySystemComponent* AbilitySystemB = FilterTask->ContextB.Context == EFPTargetFilterTaskContext::Source ? SourceAbilitySystem : TargetAbilitySystem;

	if (FilterTask->ContextA.IsValid(AbilitySystemA))
	{
		if (FilterTask->ContextA.ValueType == EFPAttributeComparison_ValueType::AttributeValue)
		{
			BindingA.Attribute = FilterTask->ContextA.AttributeValue.Attribute;
			BindingA.Handle = AbilitySystemA->GetGameplayAttributeValueChangeDelegate(BindingA.Attribute).AddRaw(this, &FFPTargetFilterObserver_AttributeComparison::OnAttributeChanged);
			BindingA.AbilitySystem = AbilitySystemA;
		}
	}

	if (FilterTask->ContextB.IsValid(AbilitySystemB))
	{
		if (FilterTask->ContextB.ValueType == EFPAttributeComparison_ValueType::AttributeValue)
		{
			BindingB.Attribute = FilterTask->ContextB.AttributeValue.Attribute;
			BindingB.Handle = AbilitySystemB->GetGameplayAttributeValueChangeDelegate(BindingB.Attribute).AddRaw(this, &FFPTargetFilterObserver_AttributeComparison::OnAttributeChanged);
			BindingB.AbilitySystem = AbilitySystemB;
		}
	}
}

void FFPTargetFilterObserver_AttributeComparison::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	CheckResultChanged();
}
