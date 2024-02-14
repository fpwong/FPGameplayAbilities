// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/FPGameplayValueRow.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

float UFPValueCalculation::Calculate(UAbilitySystemComponent* ASC, float BaseValue, const FGameplayTagContainer& Tags)
{
	return BaseValue;
}

bool UFPGameplayValueHelpers::GetBaseValueFromTable(UDataTable* DataTable, FGameplayTag Tag, float& Value)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr))
		{
			Value = Row->Value;
			return true;
		}
	}

	return false;
}

bool UFPGameplayValueHelpers::GetTransformedValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayTagContainer EffectTags, float& OutTransformedValue)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr))
		{
			const float BaseValue = Row->Value;

			if (Row->ValueCalculation && ASC)
			{
				OutTransformedValue = Row->ValueCalculation.GetDefaultObject()->Calculate(ASC, BaseValue, EffectTags);
			}
			else
			{
				OutTransformedValue = BaseValue;
			}

			return true;
		}
	}

	return false;
}
