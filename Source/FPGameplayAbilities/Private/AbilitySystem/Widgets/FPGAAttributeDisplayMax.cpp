// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAAttributeDisplayMax.h"

#include "AbilitySystemComponent.h"

void UFPGAAttributeDisplayMax::BindToAbilitySystem(UAbilitySystemComponent* AbilitySystem)
{
	if (!AbilitySystem || !CurrentAttribute.IsValid() || !MaxAttribute.IsValid())
	{
		return;
	}

	UnbindAbilitySystem();

	AbilitySystem->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddUObject(this, &UFPGAAttributeDisplayMax::OnAttributeChanged);
	AbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UFPGAAttributeDisplayMax::OnAttributeChanged);

	BoundAbilitySystem = AbilitySystem;

	UpdateText();
}

void UFPGAAttributeDisplayMax::BindAndSetAttributes(UAbilitySystemComponent* AbilitySystem, const FGameplayAttribute& Current, const FGameplayAttribute& Max)
{
	CurrentAttribute = Current;
	MaxAttribute = Max;
	BindToAbilitySystem(AbilitySystem);
}

void UFPGAAttributeDisplayMax::UpdateText()
{
	if (CurrentAttribute.IsValid() && MaxAttribute.IsValid())
	{
		const int Current = FMath::CeilToInt(BoundAbilitySystem->GetNumericAttribute(CurrentAttribute));
		const int Max = FMath::CeilToInt(BoundAbilitySystem->GetNumericAttribute(MaxAttribute));
		SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Current, Max)));
	}
}

void UFPGAAttributeDisplayMax::OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UpdateText();
}

void UFPGAAttributeDisplayMax::UnbindAbilitySystem()
{
	if (BoundAbilitySystem.IsValid() && CurrentAttribute.IsValid() && MaxAttribute.IsValid())
	{
		BoundAbilitySystem->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).RemoveAll(this);
		BoundAbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute).RemoveAll(this);
	}
}

void UFPGAAttributeDisplayMax::BeginDestroy()
{
	UnbindAbilitySystem();

	Super::BeginDestroy();
}
