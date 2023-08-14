// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAAttributeBar.h"
#include "AbilitySystemComponent.h"

void UFPGAAttributeBar::BindAttributes(UAbilitySystemComponent* AbilitySystem)
{
	// check if we are bound to old delegates
	UnbindAttributes();

	// bind to the new ability system
	AbilitySystemPtr = AbilitySystem;

	if (AbilitySystemPtr != nullptr && CurrentAttribute.IsValid())
	{
		check(AbilitySystemPtr->IsValidLowLevel());

		bNeedToUnbind = true;

		CurrentDelegateHandle = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddUObject(this, &UFPGAAttributeBar::OnCurrentAttributeChanged);

		CurrentValue = AbilitySystemPtr->GetNumericAttribute(CurrentAttribute);

		if (MaxAttribute.IsValid())
		{
			MaxDelegateHandle = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UFPGAAttributeBar::OnMaxAttributeChanged);
			MaxValue = AbilitySystemPtr->GetNumericAttribute(MaxAttribute);
		}
		
		UpdatePercent();
	}
}

void UFPGAAttributeBar::BindAndSetAttributes(UAbilitySystemComponent* AbilitySystem, FGameplayAttribute NewCurrent, FGameplayAttribute NewMax)
{
	CurrentAttribute = NewCurrent;
	MaxAttribute = NewMax;

	BindAttributes(AbilitySystem);
}

void UFPGAAttributeBar::UnbindAttributes()
{
	if (bNeedToUnbind && AbilitySystemPtr != nullptr && AbilitySystemPtr->IsValidLowLevel() && CurrentAttribute.IsValid())
	{
		if (CurrentDelegateHandle.IsValid())
		{
			FOnGameplayAttributeValueChange& Delegate = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(CurrentAttribute);
			if (Delegate.IsBound())
			{
				Delegate.Remove(CurrentDelegateHandle);
			}
		}

		if (MaxAttribute.IsValid() && MaxDelegateHandle.IsValid())
		{
			FOnGameplayAttributeValueChange& Delegate = AbilitySystemPtr->GetGameplayAttributeValueChangeDelegate(MaxAttribute);
			if (Delegate.IsBound())
			{
				Delegate.Remove(MaxDelegateHandle);
			}
		}
	}
}

void UFPGAAttributeBar::OnCurrentAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentValue = ChangeData.NewValue;
	UpdatePercent();
}

void UFPGAAttributeBar::OnMaxAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	MaxValue = ChangeData.NewValue;
	UpdatePercent();
}

void UFPGAAttributeBar::SetMaxValue(float NewMaxValue)
{
	MaxValue = NewMaxValue;
	UpdatePercent();
}

void UFPGAAttributeBar::UpdatePercent()
{
	const float Max = GetMaxValue();
	const bool bIsZero = CurrentValue == 0 || Max == 0;
	const float NewPercent = bIsZero ? 0 : CurrentValue / Max;
	const float OldPercent = NewPercent;

 	SetPercent(NewPercent);

	OnFPGAAttributeBarChanged.Broadcast(OldPercent, NewPercent);
}

void UFPGAAttributeBar::BeginDestroy()
{
	UnbindAttributes();

	UProgressBar::BeginDestroy();
}

float UFPGAAttributeBar::GetMaxValue_Implementation()
{
	return MaxValue;
}

#if WITH_EDITOR
const FText UFPGAAttributeBar::GetPaletteCategory()
{
	return FText::FromString("Ability System");
}
#endif
