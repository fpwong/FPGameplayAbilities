﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/FPGameplayValueRow.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"

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

bool UFPGameplayValueHelpers::GetDisplayValueFromTable(UDataTable* DataTable, FGameplayTag Tag, FString& OutString)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr))
		{
			if (Row->ValueDisplayMethod)
			{
				OutString = Row->ValueDisplayMethod->GetDefaultObject<UFPValueDisplay>()->GetDisplayString(Row->Value);
			}
			else
			{
				OutString = FString::SanitizeFloat(Row->Value, 0);
				// UE_LOG(LogTemp, Warning, TEXT("%f %s"), Row->Value, *OutString);
			}

			return true;
		}
	}

	return false;
}

bool UFPGameplayValueHelpers::GetTransformedDisplayValueFromTable(UDataTable* DataTable, FGameplayTag Tag, UAbilitySystemComponent* ASC, FGameplayTagContainer EffectTags, FString& OutString)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr))
		{
			float Value = Row->Value;
			if (ASC && Row->ValueCalculation)
			{
				Value = Row->ValueCalculation.GetDefaultObject()->Calculate(ASC, Row->Value, EffectTags);
			}

			if (Row->ValueDisplayMethod)
			{
				OutString = Row->ValueDisplayMethod->GetDefaultObject<UFPValueDisplay>()->GetDisplayString(Value);
			}
			else
			{
				OutString = FString::SanitizeFloat(Value, 0);
			}

			return true;
		}
	}

	return false;
}

void UFPGameplayValueHelpers::ApplyGameValueTableToSpec(UAbilitySystemComponent* ASC, FGameplayEffectSpecHandle SpecHandle, UDataTable* DataTable)
{
	TSharedPtr<FGameplayEffectSpec> Spec = SpecHandle.Data;
	if (!Spec)
	{
		return;
	}

	// grab gameplay tags from GameplayEffect
	TArray<FGameplayTag> SetByCallerTags = UFPGAGameplayAbilitiesLibrary::GetSetByCallerTagsFromGameplayEffect(Spec->Def);


	// look for the tag in the data table
	for (const FGameplayTag& Tag : SetByCallerTags)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr))
		{
			// FGameplayTagContainer& EffectTags = Spec->CapturedSourceTags.GetSpecTags();
			// FGameplayTagContainer& EffectTags;
			FGameplayTagContainer EffectTags;
			Spec->GetAllAssetTags(EffectTags);

			// make a container from array
			FGameplayTagContainer RowTags = FGameplayTagContainer::CreateFromArray(Row->Tags);
			EffectTags.AppendTags(RowTags);

			// UE_LOG(LogTemp, Warning, TEXT("HUH Tags %s"), *EffectTags.ToStringSimple());

			// UFPGameplayValueHelpers::MantisGetTagsFromAbilityAndTable();

			// append row tags to the spec
			Spec->AppendDynamicAssetTags(EffectTags);

			const float BaseValue = Row->Value;

			if (!Row->bValueCalculationOnlyForDisplayValue && Row->ValueCalculation && ASC)
			{
				const float TransformedValue = Row->ValueCalculation.GetDefaultObject()->Calculate(ASC, BaseValue, EffectTags);
				Spec->SetSetByCallerMagnitude(Tag, TransformedValue);
			}
			else
			{
				Spec->SetSetByCallerMagnitude(Tag, BaseValue);
			}
		}
		else
		{
			FMessageLog PIELogger = FMessageLog(FName("PIE"));
			PIELogger.Error(FText::Format(INVTEXT("Failed to find tag from table {0}"), FText::FromName(Tag.GetTagName())));
			// UE_LOG(LogTemp, Warning, TEXT("Failed to find tag from table %s"), *Tag.GetTagName().ToString());
		}

		// float TransformedValue = 0.0f;
		// if (GetTransformedValueFromTable(DataTable, ASC, Tag, EffectTags, TransformedValue))
		// {
		// 	// UE_LOG(LogTemp, Warning, TEXT("Set %s to %f"), *Tag.GetTagName().ToString(), TransformedValue);
		// 	Spec->SetSetByCallerMagnitude(Tag, TransformedValue);
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Failed to find tag from table %s"), *Tag.GetTagName().ToString());
		// }
	}
}

FString UFPGameplayValueHelpers::ApplyValueDisplayMethod(TSubclassOf<UFPValueDisplay> ValueDisplayMethod, float Value)
{
	if (ValueDisplayMethod)
	{
		return ValueDisplayMethod.GetDefaultObject()->GetDisplayString(Value);
	}

	return "";
}
