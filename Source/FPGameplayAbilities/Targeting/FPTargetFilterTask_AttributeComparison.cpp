// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterTask_AttributeComparison.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"

bool UFPTargetFilterTask_AttributeComparison::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return true;
	}

	UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!TargetAbilitySystem)
	{
		return false;
	}

	const float ValueA = TargetAbilitySystem->GetNumericAttribute(Attribute);
	bool bResult = false;

	const float ValueB = GetValue();

	UE_LOG(LogTemp, Warning, TEXT("Value %f"), ValueB);

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

float UFPTargetFilterTask_AttributeComparison::GetValue() const
{
	switch (ValueType)
	{
		case EFPAttributeComparison_ValueType::StaticValue:
			return StaticValue;
		case EFPAttributeComparison_ValueType::AttributeValue:
		{
			TArray<FGameplayEffectAttributeCaptureDefinition> CaptureDefs;
			AttributeValue.GetAttributeCaptureDefinitions(CaptureDefs);

			FGameplayEffectSpec Spec;
			// FGameplayEffectAttributeCaptureDefinition CaptureDef(AttributeValue.GetAttributeCaptureDefinitions());
			for (const FGameplayEffectAttributeCaptureDefinition& Def : CaptureDefs)
			{
				Spec.CapturedRelevantAttributes.AddCaptureDefinition(Def);
			}
			float OutValue;
			AttributeValue.AttemptCalculateMagnitude(Spec, OutValue);
			return OutValue;
		}
		default:
			return 0.0f;
	}
}

FFPTargetFilterObserver* UFPTargetFilterTask_AttributeComparison::MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	FFPTargetFilterObserver_AttributeComparison* Binding = new FFPTargetFilterObserver_AttributeComparison();
	Binding->InitAttributeValue(Cast<UFPTargetFilterTask_AttributeComparison>(FilterTask), SourceActor, TargetActor);
	return Binding;
}

FFPTargetFilterObserver_AttributeComparison::~FFPTargetFilterObserver_AttributeComparison()
{
	if (AbilitySystem.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(AttributeChangedHandle);
		AttributeChangedHandle.Reset();
	}
}

void FFPTargetFilterObserver_AttributeComparison::InitAttributeValue(UFPTargetFilterTask_AttributeComparison* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	Init(FilterTask, SourceActor, TargetActor);

	AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!AbilitySystem.IsValid())
	{
		return;
	}

	Attribute = FilterTask->Attribute;
	AttributeChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(FilterTask->Attribute).AddRaw(this, &FFPTargetFilterObserver_AttributeComparison::OnAttributeChanged);
}

void FFPTargetFilterObserver_AttributeComparison::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	CheckResultChanged();
}
