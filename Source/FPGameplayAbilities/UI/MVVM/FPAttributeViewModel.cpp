// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAttributeViewModel.h"

#include "AbilitySystemComponent.h"


UObject* UFPAttributeResolver::CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const
{
	return UFPAttributeViewModel::CreateInstance(GetAbilitySystem(UserWidget), Attribute);
}


UFPAttributeViewModel* UFPAttributeViewModel::CreateInstance(UAbilitySystemComponent* InASC, FGameplayAttribute InAttribute)
{
	UFPAttributeViewModel* Inst = NewObject<UFPAttributeViewModel>(InASC);
	Inst->Init(InASC, InAttribute);
	return Inst;
}

void UFPAttributeViewModel::OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	SetAttributeValue(OnAttributeChangeData.NewValue);
}

void UFPAttributeViewModel::Init(UAbilitySystemComponent* InASC, FGameplayAttribute InAttribute)
{
	AbilitySystemPtr = InASC;
	Attribute = InAttribute;

	if (AbilitySystemPtr.Get() && Attribute.IsValid())
	{
		DelegateHandle = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UFPAttributeViewModel::OnAttributeChanged);
		SetAttributeValue(AbilitySystemPtr.Get()->GetNumericAttribute(Attribute));
	}
}

void UFPAttributeViewModel::SetAttributeValue(float NewValue)
{
	AttributeValue = NewValue;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(AttributeValue);
}
