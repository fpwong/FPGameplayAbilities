

#include "AbilitySystem/Tasks/FPGAAsyncTask_AttributeChanged.h"

UFPGAAsyncTask_AttributeChanged* UFPGAAsyncTask_AttributeChanged::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
	{
		return nullptr;
	}

	UFPGAAsyncTask_AttributeChanged* WaitForAttributeChangedTask = NewObject<UFPGAAsyncTask_AttributeChanged>();
	WaitForAttributeChangedTask->Init(AbilitySystemComponent, Attribute);
	WaitForAttributeChangedTask->BindAttributeChanged();
	return WaitForAttributeChangedTask;
}

UFPGAAsyncTask_AttributeChanged* UFPGAAsyncTask_AttributeChanged::ListenForAttributeChangeWithTags(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute, FGameplayTagContainer Tags)
{
	if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
	{
		return nullptr;
	}

	UFPGAAsyncTask_AttributeChanged* WaitForAttributeChangedTask = NewObject<UFPGAAsyncTask_AttributeChanged>();
	WaitForAttributeChangedTask->Init(AbilitySystemComponent, Attribute);
	WaitForAttributeChangedTask->BindAttributeChangedWithTags(Tags);
	return WaitForAttributeChangedTask;
}

void UFPGAAsyncTask_AttributeChanged::EndTask()
{
	if (IsValid(ASC))
	{
		if (IsValid(ASC->GetOwner()))
		{
			ASC->GetOwner()->OnDestroyed.RemoveAll(this);
		}

		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UFPGAAsyncTask_AttributeChanged::Init(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute)
{
	ASC = AbilitySystemComponent;
	AttributeToListenFor = Attribute;
	ASC->GetOwner()->OnDestroyed.AddDynamic(this, &UFPGAAsyncTask_AttributeChanged::OnActorDestroyed);
}

void UFPGAAsyncTask_AttributeChanged::BindAttributeChanged()
{
	ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).AddUObject(this, &UFPGAAsyncTask_AttributeChanged::AttributeChanged);
}

void UFPGAAsyncTask_AttributeChanged::BindAttributeChangedWithTags(const FGameplayTagContainer& Tags)
{
	const FGameplayEffectAttributeCaptureDefinition Capture(AttributeToListenFor, EGameplayEffectAttributeCaptureSource::Source, false);
	CaptureSpec = FGameplayEffectAttributeCaptureSpec(Capture);
	ASC->CaptureAttributeForGameplayEffect(CaptureSpec);

	EvalTags = Tags;
	EvalParams.SourceTags = &EvalTags;

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).AddUObject(this, &UFPGAAsyncTask_AttributeChanged::AttributeChangedWithTags);

	OldCalculatedValue = CalculateAttributeValue();
}

void UFPGAAsyncTask_AttributeChanged::AttributeChanged(const FOnAttributeChangeData & Data)
{
	OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}

void UFPGAAsyncTask_AttributeChanged::AttributeChangedWithTags(const FOnAttributeChangeData& Data)
{
	const float NewValue = CalculateAttributeValue();
	if (NewValue != OldCalculatedValue)
	{
		OnAttributeChanged.Broadcast(Data.Attribute, NewValue, OldCalculatedValue);
		OldCalculatedValue = NewValue;
	}
}

float UFPGAAsyncTask_AttributeChanged::CalculateAttributeValue()
{
	if (EvalTags.IsEmpty())
	{
		return ASC->GetNumericAttribute(AttributeToListenFor);
	}
	else
	{
		float RetVal = 0;
		CaptureSpec.AttemptCalculateAttributeMagnitude(EvalParams, RetVal);
		return RetVal;
	}
}

void UFPGAAsyncTask_AttributeChanged::OnActorDestroyed(AActor* DestroyedActor)
{
	EndTask();
}
