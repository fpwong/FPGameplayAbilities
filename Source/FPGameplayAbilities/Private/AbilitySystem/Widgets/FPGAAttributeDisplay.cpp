// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAAttributeDisplay.h"
#include "AbilitySystemComponent.h"

UFPGAAttributeDisplay::UFPGAAttributeDisplay()
{
	Text = FText::FromString("0");
}

void UFPGAAttributeDisplay::BindAttribute(UAbilitySystemComponent* AbilitySystem)
{
	UnbindAttribute();

	AbilitySystemPtr = AbilitySystem;

	if (AbilitySystemPtr.Get() && Attribute.IsValid())
	{
		DelegateHandle = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UFPGAAttributeDisplay::OnAttributeChanged);
		bNeedToUnbind = true;

		const float AttributeValue = AbilitySystemPtr->GetNumericAttribute(Attribute);
		// const int Ceil = FMath::CeilToInt(AttributeValue);
		// const FText AttributeText = FText::FromString(FString::Printf(TEXT("%.1f"), AttributeValue));

		static const FNumberFormattingOptions NumberFormat = FNumberFormattingOptions()
															 .SetMinimumFractionalDigits(0)
															 .SetMaximumFractionalDigits(3);

		const FText AttributeText = FText::AsNumber(AttributeValue, &NumberFormat);

		SetText(AttributeText);
		OnAttributeDisplayChanged.Broadcast(0.f, AttributeValue);
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
			UE_LOG(LogTemp, Error, TEXT("%s: Delegate Handle Invalid"), TEXT(__FUNCTION__));
		}
	}

	DelegateHandle.Reset();
	AbilitySystemPtr.Reset();
}

void UFPGAAttributeDisplay::OnAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	// const FText AttributeText = FText::FromString(FString::Printf(TEXT("%f"), ChangeData.NewValue));

	static const FNumberFormattingOptions NumberFormat = FNumberFormattingOptions()
														 .SetMinimumFractionalDigits(0)
														 .SetMaximumFractionalDigits(3);

	const FText AttributeText = FText::AsNumber(ChangeData.NewValue, &NumberFormat);

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
