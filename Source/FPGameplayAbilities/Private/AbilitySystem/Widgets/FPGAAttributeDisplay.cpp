// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAAttributeDisplay.h"
#include "AbilitySystemComponent.h"

void UFPGAAttributeDisplay::BindAttribute(UAbilitySystemComponent* AbilitySystem)
{
	UnbindAttribute();

	AbilitySystemPtr = AbilitySystem;

	if (AbilitySystemPtr != nullptr && Attribute.IsValid())
	{
		DelegateHandle = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UFPGAAttributeDisplay::OnAttributeChanged);
		bNeedToUnbind = true;

		const float AttributeValue = AbilitySystemPtr->GetNumericAttribute(Attribute);
		const FText AttributeText = FText::FromString(FString::Printf(TEXT("%.0f"), AttributeValue));
		SetText(AttributeText);
		OnAttributeDisplayChanged.Broadcast(0.f, AttributeValue);
	}
}

void UFPGAAttributeDisplay::BindAndSetAttribute(UAbilitySystemComponent* AbilitySystem, FGameplayAttribute NewAttribute)
{
	Attribute = NewAttribute;
	BindAttribute(AbilitySystem);
}

void UFPGAAttributeDisplay::UnbindAttribute()
{
	//if (!bNeedToUnbind)
	//{
	//	return;
	//}

	//if (AbilitySystemPtr == nullptr || AbilitySystemPtr->IsBeingDestroyed() || AbilitySystemPtr->IsPendingKill())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("ABILITY SYSTEM DIED"));
	//	return;
	//}


	if (AbilitySystemPtr == nullptr)
		return;

	//if (Attribute.IsValid() && AbilitySystemPtr->GetOwner() != nullptr)
	{
		if (DelegateHandle.IsValid())
		{
			//bNeedToUnbind = false;
			AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(DelegateHandle);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Delegate Handle Invalid"), TEXT(__FUNCTION__));
		}
	}
	/*else
	{
		UE_LOG(LogTemp, Warning, TEXT("RISETONSIETNRSTNOISET"));
	}*/

	DelegateHandle.Reset();
	AbilitySystemPtr = nullptr;
}

void UFPGAAttributeDisplay::OnAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%.0f"), ChangeData.NewValue));
	const float OldValue = ChangeData.OldValue;
 	SetText(AttributeText);
	OnAttributeDisplayChanged.Broadcast(ChangeData.OldValue, ChangeData.NewValue);
}

void UFPGAAttributeDisplay::BeginDestroy()
{
	UnbindAttribute();

	UTextBlock::BeginDestroy();
}

#if WITH_EDITOR
const FText UFPGAAttributeDisplay::GetPaletteCategory()
{
	return FText::FromString("Ability System");
}
#endif