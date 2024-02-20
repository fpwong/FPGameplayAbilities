// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGameplayEffectUIData.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "Misc/FPGameplayValueRow.h"

FText FFPDynamicValueDescription::GetText() const
{
	FFormatOrderedArguments Args;

	for (const FGameplayTag& Tag : DescriptionArguments)
	{
		FString DisplayValue;
		if (UFPGameplayValueHelpers::GetDisplayValueFromTable(DataTable, Tag, DisplayValue))
		{
			Args.Add(FText::FromString(DisplayValue));
		}
	}


	return FText::Format(DynamicDescription, Args);
}

FText UFPGameplayUIHelpers::EvaluateDynamicDescription(const FFPDynamicValueDescription& Description)
{
	return Description.GetText();
}
