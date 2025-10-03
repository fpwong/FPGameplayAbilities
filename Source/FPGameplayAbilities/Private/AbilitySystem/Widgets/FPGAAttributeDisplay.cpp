// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAAttributeDisplay.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "FPGameplayAbilities/FPGASettings.h"

UFPGAAttributeDisplayDataAsset& UFPGAAttributeDisplayDataAsset::Get()
{
	return *UFPGASettings::Get().AttributeDisplayData.LoadSynchronous();
}

FString UFPGAAttributeDisplayDataAsset::GetAttributeName(FGameplayAttribute Attribute)
{
	if (FFPAttributeDisplayData* Display = Get().DisplayData.Find(Attribute))
	{
		return Display->Name.ToString();
	}

	return Attribute.AttributeName;
}

FString UFPGAAttributeDisplayDataAsset::GetAttributeValueAsString(FGameplayAttribute Attribute, float AttributeValue)
{
	if (FFPAttributeDisplayData* Display = Get().DisplayData.Find(Attribute))
	{
		TStringBuilder<256> StringBuilder;
		if (Display->ValueDisplayMethod)
		{
			StringBuilder.Append(UFPGameplayValueHelpers::ApplyValueDisplayMethod(Display->ValueDisplayMethod, AttributeValue));
		}
		else
		{
			FNumberFormattingOptions Format;
			Format.MinimumFractionalDigits = Display->MinNumDecimals;
			Format.MaximumFractionalDigits = Display->MaxNumDecimals;
			StringBuilder.Append(FText::AsNumber(AttributeValue, &Format).ToString());
		}

		// Add Suffix: Unit
		if (!Display->Unit.IsEmpty())
		{
			StringBuilder.Append(Display->Unit);
		}

		return StringBuilder.ToString();
	}

	return FString::SanitizeFloat(AttributeValue);
}

UFPGAAttributeDisplay::UFPGAAttributeDisplay()
{
}

void UFPGAAttributeDisplay::BindAttribute(UAbilitySystemComponent* AbilitySystem)
{
	const auto& Display = GetDisplayData();
	NumberFormat.SetMinimumFractionalDigits(Display.MinNumDecimals).SetMaximumFractionalDigits(Display.MaxNumDecimals).SetRoundingMode(ToZero);

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

	auto& Display = GetDisplayData();

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
	else if (Display.ValueDisplayMethod)
	{
		ValueString = UFPGameplayValueHelpers::ApplyValueDisplayMethod(Display.ValueDisplayMethod, AttributeValue);
	}
	else
	{
		ValueString = FText::AsNumber(AttributeValue, &NumberFormat).ToString();
	}

	TStringBuilder<256> StringBuilder;

	if (bShowAttributeName)
	{
		if (AttributeTags.Num())
		{
			auto TagArr = AttributeTags.GetGameplayTagArray();
			for (int i = 0; i < TagArr.Num(); ++i)
			{
				auto Tag = TagArr[i];
				StringBuilder.Append(Tag.GetTagLeafName().ToString());

				if (i != TagArr.Num() - 1)
				{
					StringBuilder.Append(TEXT(","));
				}
				else
				{
					StringBuilder.Append(TEXT(" "));
				}
			}
		}

		FString AttribName = Display.Name.IsEmpty() ? Attribute.AttributeName : Display.Name.ToString();  

		StringBuilder.Append(AttribName);
		StringBuilder.Append(TEXT(": "));
	}

	if (bShowPlusMinus && AttributeValue > 0)
	{
		StringBuilder.AppendChar(TEXT('+'));
	}
	// A negative sign is already part of the string from SanitizeFloat.

	StringBuilder.Append(ValueString);

	// Add Suffix: Unit
	if (!Display.Unit.IsEmpty())
	{
		// StringBuilder.AppendChar(TEXT(' '));
		StringBuilder.Append(Display.Unit);
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

const FFPAttributeDisplayData& UFPGAAttributeDisplay::GetDisplayData()
{
	if (!UFPGASettings::Get().AttributeDisplayData.IsNull())
	{
		if (UFPGAAttributeDisplayDataAsset* AttribData = Cast<UFPGAAttributeDisplayDataAsset>(UFPGASettings::Get().AttributeDisplayData.LoadSynchronous()))
		{
			if (FFPAttributeDisplayData* Data = AttribData->DisplayData.Find(Attribute))
			{
				return *Data;
			}
		}
	}

	return DisplayData;
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
