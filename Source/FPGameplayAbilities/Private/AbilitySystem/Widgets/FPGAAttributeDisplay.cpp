// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAAttributeDisplay.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

UFPGAAttributeDisplay::UFPGAAttributeDisplay()
{
}

void UFPGAAttributeDisplay::BindAttribute(UAbilitySystemComponent* AbilitySystem)
{
	NumberFormat.SetMinimumFractionalDigits(MinNumDecimals).SetMaximumFractionalDigits(MaxNumDecimals).SetRoundingMode(ToZero);

	UnbindAttribute();

	AbilitySystemPtr = AbilitySystem;

	if (AbilitySystemPtr.Get() && Attribute.IsValid())
	{
		DelegateHandle = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UFPGAAttributeDisplay::OnAttributeChanged);
		bNeedToUnbind = true;

		UpdateAttributeValue(false);
	}
	else
	{
		SetText(FText::FromString("-"));
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

	if (!AbilitySystemPtr.IsValid())
	{
		return;
	}

	if (Attribute.IsValid() && AbilitySystemPtr->GetOwner() != nullptr)
	{
		if (DelegateHandle.IsValid())
		{
			//bNeedToUnbind = false;
			AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(DelegateHandle);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s: Delegate Handle Invalid"), ANSI_TO_TCHAR(__FUNCTION__));
		}
	}

	DelegateHandle.Reset();
	AbilitySystemPtr.Reset();
}

void UFPGAAttributeDisplay::OnAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	UpdateAttributeValue(true);
}

void UFPGAAttributeDisplay::UpdateAttributeValue(bool bBroadcastChange)
{
	const float AttributeValue = UFPGAGameplayAbilitiesLibrary::GetAttributeValueWithTags(AbilitySystemPtr.Get(), Attribute, AttributeTags);

	FText AttributeText = FText::AsNumber(AttributeValue, &NumberFormat);

	if (ValueDisplayMethod)
	{
		AttributeText = FText::FromString(UFPGameplayValueHelpers::ApplyValueDisplayMethod(ValueDisplayMethod, AttributeValue));
	}

	SetText(AttributeText);

	if (OldValue != AttributeValue)
	{
		if (bBroadcastChange)
		{
			OnAttributeDisplayChanged.Broadcast(OldValue, AttributeValue);
		}

		OldValue = AttributeValue;
	}
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
