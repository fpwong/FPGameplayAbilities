// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAAttributeDisplayMax.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

void UFPGAAttributeDisplayMax::SetMaxValue(float NewMaxValue)
{
	MaxValue = NewMaxValue;
	UpdateText();
}

void UFPGAAttributeDisplayMax::BindToAbilitySystem(UAbilitySystemComponent* AbilitySystem)
{
	if (!AbilitySystem || !CurrentAttribute.IsValid())
	{
		return;
	}

	UnbindAbilitySystem();

	AbilitySystem->GetGameplayAttributeValueChangeDelegate(CurrentAttribute).AddUObject(this, &UFPGAAttributeDisplayMax::OnAttributeChanged);

	if (MaxAttribute.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UFPGAAttributeDisplayMax::OnMaxAttributeChanged);
		MaxValue = AbilitySystem->GetNumericAttribute(MaxAttribute);
	}

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
	if (CurrentAttribute.IsValid())
	{
		const int Current = FMath::CeilToInt(UFPGAGameplayAbilitiesLibrary::GetAttributeValueWithTags(BoundAbilitySystem.Get(), CurrentAttribute, CurrentAttributeTags));
		SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Current, FMath::CeilToInt(MaxValue))));
		// UE_LOG(LogTemp, Warning, TEXT("Update %s %s"), *CurrentAttribute.AttributeName, *FString::Printf(TEXT("%d/%d"), Current, FMath::CeilToInt(MaxValue)));
	}
}

void UFPGAAttributeDisplayMax::OnAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UpdateText();
}

void UFPGAAttributeDisplayMax::OnMaxAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxValue = OnAttributeChangeData.NewValue;
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
