// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/FPGameplayValueRow.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "AbilitySystem/FPGAGameplayAbilityInterface.h"
#include "AbilitySystem/FPGATypes.h"

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

			if (Row->Settings && Row->Settings->ValueCalculation && ASC)
			{
				OutTransformedValue = Row->Settings->ValueCalculation.GetDefaultObject()->Calculate(ASC, BaseValue, EffectTags);
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
			if (Row->Settings && Row->Settings->ValueDisplayMethod)
			{
				OutString = Row->Settings->ValueDisplayMethod->GetDefaultObject<UFPValueDisplay>()->GetDisplayString(Row->Value);
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

bool UFPGameplayValueHelpers::GetTransformedDisplayValueFromTable(UDataTable* DataTable, FGameplayTag Tag, UAbilitySystemComponent* ASC, FGameplayTagContainer EffectTags, FString& OutString, FFPGameplayValueRow& OutRow)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr))
		{
			float Value = Row->Value;
			if (ASC && Row->Settings && Row->Settings->ValueCalculation)
			{
				Value = Row->Settings->ValueCalculation.GetDefaultObject()->Calculate(ASC, Row->Value, EffectTags);
			}

			if (Row->Settings && Row->Settings->ValueDisplayMethod)
			{
				OutString = Row->Settings->ValueDisplayMethod->GetDefaultObject<UFPValueDisplay>()->GetDisplayString(Value);
			}
			else
			{
				OutString = FString::SanitizeFloat(Value, 0);
			}

			OutRow = *Row;

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

	FGameplayTagContainer EffectTags;
	Spec->GetAllAssetTags(EffectTags);

	// look for the tag in the data table
	for (const FGameplayTag& Tag : SetByCallerTags)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr))
		{
			// FGameplayTagContainer& EffectTags = Spec->CapturedSourceTags.GetSpecTags();
			// FGameplayTagContainer& EffectTags;

			// make a container from array
			FGameplayTagContainer LocalTags = MoveTempIfPossible(EffectTags);
			LocalTags.AppendTags(FGameplayTagContainer::CreateFromArray(Row->Tags));

			// UE_LOG(LogTemp, Warning, TEXT("HUH Tags %s"), *EffectTags.ToStringSimple());

			// UFPGameplayValueHelpers::MantisGetTagsFromAbilityAndTable();

			// append row tags to the spec
			Spec->AppendDynamicAssetTags(LocalTags);

			const float BaseValue = Row->Value;

			if (Row->Settings && !Row->Settings->bUseCalculationOnlyForDisplayValue && Row->Settings->ValueCalculation && ASC)
			{
				const float TransformedValue = Row->Settings->ValueCalculation.GetDefaultObject()->Calculate(ASC, BaseValue, LocalTags);
				Spec->SetSetByCallerMagnitude(Tag, TransformedValue);
			}
			else
			{
				Spec->SetSetByCallerMagnitude(Tag, BaseValue);
			}
		}
		// else
		// {
		// 	FMessageLog PIELogger = FMessageLog(FName("PIE"));
		// 	PIELogger.Error(FText::Format(INVTEXT("Failed to find tag from table {0}"), FText::FromName(Tag.GetTagName())));
		// 	// UE_LOG(LogTemp, Warning, TEXT("Failed to find tag from table %s"), *Tag.GetTagName().ToString());
		// }

		// set period
		if (Spec->Def)
		{
			if (Spec->Def->Implements<UFPGameplayEffectInterface>())
			{
				if (UGameplayEffect* MutGE = const_cast<UGameplayEffect*>(Spec->Def.Get()))
				{
					FGameplayTag PeriodTag = IFPGameplayEffectInterface::Execute_GetPeriodValueTag(MutGE, ASC);
					if (PeriodTag.IsValid())
					{
						if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(PeriodTag.GetTagName(), nullptr))
						{
							float Period = Row->Value;

							if (Row->Settings && !Row->Settings->bUseCalculationOnlyForDisplayValue && Row->Settings->ValueCalculation && ASC)
							{
								Period = Row->Settings->ValueCalculation.GetDefaultObject()->Calculate(ASC, Period, EffectTags);
							}

							Spec->Period = Period;
						}
					}
				}
			}
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

FGameplayTagContainer UFPGameplayValueHelpers::GatherTagsFromGameplayAbility(UGameplayAbility* GameplayAbility, FGameplayTag GameValueTag, UDataTable* DataTable)
{
	FGameplayTagContainer Tags;
	if (GameplayAbility->Implements<UFPAbilityInterface>())
	{
		if (IFPAbilityInterface* AsInterface = Cast<IFPAbilityInterface>(GameplayAbility))
		{
			Tags = AsInterface->GetAbilityTypeTags();
		}
	}

	if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(GameValueTag.GetTagName(), nullptr))
	{
		// make a container from array
		FGameplayTagContainer RowTags = FGameplayTagContainer::CreateFromArray(Row->Tags);
		Tags.AppendTags(RowTags);
	}

	return Tags;
}

FString UFPGameplayValueHelpers::ApplyValueDisplayMethod(TSubclassOf<UFPValueDisplay> ValueDisplayMethod, float Value)
{
	if (ValueDisplayMethod)
	{
		return ValueDisplayMethod.GetDefaultObject()->GetDisplayString(Value);
	}

	return "";
}

bool UFPGameplayValueHelpers::DoesGameValueTableHaveTag(FGameplayTag GameValueTag, UDataTable* DataTable)
{
	return DataTable->FindRowUnchecked(GameValueTag.GetTagName()) != nullptr;
}
