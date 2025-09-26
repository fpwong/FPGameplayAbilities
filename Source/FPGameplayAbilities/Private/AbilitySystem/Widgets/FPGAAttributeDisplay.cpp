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

void UFPGAAttributeDisplay::BindAndSetAttributeWithTag(UAbilitySystemComponent* AbilitySystem, FGameplayAttribute NewAttribute, FGameplayTagContainer NewAttributeTags)
{
	AttributeTags = NewAttributeTags;
	BindAndSetAttribute(AbilitySystem, NewAttribute);
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
	if (!AbilitySystemPtr.IsValid())
	{
		return;
	}

	const float AttributeValue = UFPGAGameplayAbilitiesLibrary::GetAttributeValueWithTags(AbilitySystemPtr.Get(), Attribute, AttributeTags);

	FString ValueString;
	if (bShowBaseFinal)
	{
		static FGameplayTagRequirements Empty;
		bool bSuccess;
		const int32 BaseValue = UFPGAGameplayAbilitiesLibrary::EvaluateAttributeValueForChannel(AbilitySystemPtr.Get(), Attribute, BaseChannel, bSuccess);
		const int32 FinalValue = AbilitySystemPtr.Get()->GetNumericAttribute(Attribute);

		if (FinalValue != BaseValue)
		{
			ValueString = FString::Printf(TEXT("%d + %d"), BaseValue, (FinalValue - BaseValue));
		}
		else
		{
			ValueString = FText::AsNumber(AttributeValue, &NumberFormat).ToString();
		}
	}
	else if (ValueDisplayMethod)
	{
		ValueString = UFPGameplayValueHelpers::ApplyValueDisplayMethod(ValueDisplayMethod, AttributeValue);
	}
	else
	{
		ValueString = FText::AsNumber(AttributeValue, &NumberFormat).ToString();
	}

	TStringBuilder<256> StringBuilder;

	if (bShowAttributeName)
	{
		StringBuilder.Append(Attribute.AttributeName);
		StringBuilder.Append(TEXT(": "));
	}

	if (bShowPlusMinus && AttributeValue > 0)
	{
		StringBuilder.AppendChar(TEXT('+'));
	}
	// A negative sign is already part of the string from SanitizeFloat.

	StringBuilder.Append(ValueString);

	// Add Suffix: Unit
	if (!Unit.IsEmpty())
	{
		// StringBuilder.AppendChar(TEXT(' '));
		StringBuilder.Append(Unit);
	}

	SetText(FText::FromString(StringBuilder.ToString()));

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
