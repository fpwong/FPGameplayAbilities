// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCurrMaxAttributeViewModel.h"
#include "AbilitySystemComponent.h"


UObject* UFPCurrMaxAttributeResolver::CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const
{
	return UFPCurrMaxAttributeViewModel::CreateInstance(GetAbilitySystem(UserWidget), Curr, Max);
}

UFPCurrMaxAttributeViewModel* UFPCurrMaxAttributeViewModel::CreateInstance(UAbilitySystemComponent* InASC, FGameplayAttribute Curr, FGameplayAttribute Max)
{
	UFPCurrMaxAttributeViewModel* Inst = NewObject<UFPCurrMaxAttributeViewModel>(InASC);
	Inst->Init(InASC, Curr, Max);
	return Inst;
}

void UFPCurrMaxAttributeViewModel::OnCurrChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	SetCurrValue(OnAttributeChangeData.NewValue);
}

void UFPCurrMaxAttributeViewModel::OnMaxChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	SetMaxValue(OnAttributeChangeData.NewValue);
}

void UFPCurrMaxAttributeViewModel::Init(UAbilitySystemComponent* InASC, FGameplayAttribute Curr, FGameplayAttribute Max)
{
	AbilitySystemPtr = InASC;

	if (AbilitySystemPtr.Get() && Curr.IsValid())
	{
		AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Curr).AddUObject(this, &UFPCurrMaxAttributeViewModel::OnCurrChanged);
		AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Max).AddUObject(this, &UFPCurrMaxAttributeViewModel::OnMaxChanged);
		SetCurrValue(AbilitySystemPtr.Get()->GetNumericAttribute(Curr));
		SetMaxValue(AbilitySystemPtr.Get()->GetNumericAttribute(Max));
		UpdatePercent();
	}
}

void UFPCurrMaxAttributeViewModel::SetCurrValue(float NewValue)
{
	CurrValue = NewValue;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(CurrValue);
	UpdatePercent();
}

void UFPCurrMaxAttributeViewModel::SetMaxValue(float NewValue)
{
	MaxValue = NewValue;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(MaxValue);
	UpdatePercent();
}

void UFPCurrMaxAttributeViewModel::UpdatePercent()
{
	Percent = MaxValue == 0 ? 0 : CurrValue / MaxValue;
	UE_LOG(LogTemp, Warning, TEXT("Update percent %f"), Percent);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Percent);
}
