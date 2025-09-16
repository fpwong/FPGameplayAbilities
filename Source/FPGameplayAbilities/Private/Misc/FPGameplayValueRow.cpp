﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/FPGameplayValueRow.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "AbilitySystem/FPGATypes.h"
#include "FPGameplayAbilities/FPGASettings.h"

float FFPGameplayValueRow::GetValueAtLevel(int Level) const
{
	if (Level >= 0 && !ScalingCurve.IsNull())
	{
		FString Out;
		const float CurveValue = ScalingCurve.Eval(Level, Out);
		return CurveValue;
	}

	return Value;
}

float FFPGameplayValueRow::GetBaseValue(UAbilitySystemComponent* ASC, int Level) const
{
	// return GetValueAtLevel(GetScalingLevel(ASC));
	return GetValueAtLevel(Level);//GetScalingLevel(ASC));
}

int FFPGameplayValueRow::GetScalingLevel(UAbilitySystemComponent* ASC) const
{
	if (!ASC)
	{
		return 0;
	}

	// skills start at base level 0
	bool bSuccess = false;
	const FGameplayAttribute& ScalingAttribute = GetDefault<UFPGASettings>()->SkillLevelAttribute;

	// in our game we decide the level by the max of lvl of any scaling tags
	int Level = 0;
	for (const FGameplayTag& Tag : ScalingTags)
	{
		FGameplayTagContainer TargetTags;
		TargetTags.AddTagFast(Tag);

		Level = FMath::Max(Level, FMath::RoundToInt32(UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTags(ASC, ScalingAttribute, FGameplayTagContainer(Tag), TargetTags, bSuccess)));
	}

	return Level;
}

bool UFPGameplayValueHelpers::GetBaseValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, float& Value, int Level)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr, false))
		{
			Value = Row->GetBaseValue(ASC, Level);
			return true;
		}
	}

	return false;
}

bool UFPGameplayValueHelpers::GetTransformedValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, FGameplayTagContainer EffectTags, float& OutTransformedValue, int Level)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr, false))
		{
			const float BaseValue = Row->GetBaseValue(ASC, Level);

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

bool UFPGameplayValueHelpers::GetDisplayValueFromTable(UDataTable* DataTable, UAbilitySystemComponent* ASC, FGameplayTag Tag, FString& OutString, int Level)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr, false))
		{
			float BaseValue = Row->GetBaseValue(ASC, Level);

			if (Row->Settings && Row->Settings->ValueDisplayMethod)
			{
				OutString = Row->Settings->ValueDisplayMethod->GetDefaultObject<UFPValueDisplay>()->GetDisplayString(BaseValue);
			}
			else
			{
				OutString = FString::SanitizeFloat(BaseValue, 0);
				// UE_LOG(LogTemp, Warning, TEXT("%f %s"), Row->Value, *OutString);
			}

			return true;
		}
	}

	return false;
}

bool UFPGameplayValueHelpers::GetTransformedDisplayValueFromTable(UDataTable* DataTable, FGameplayTag Tag, UAbilitySystemComponent* ASC, FGameplayTagContainer EffectTags, FString& OutString, FFPGameplayValueRow& OutRow, int Level)
{
	if (DataTable)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr, false))
		{
			float Value = Row->GetBaseValue(ASC, Level);
			if (ASC && Row->Settings && Row->Settings->ValueCalculation)
			{
				Value = Row->Settings->ValueCalculation.GetDefaultObject()->Calculate(ASC, Value, EffectTags);
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

	int Level = SpecHandle.Data->GetLevel();

	// look for the tag in the data table
	for (const FGameplayTag& Tag : SetByCallerTags)
	{
		if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr, false))
		{
			// FGameplayTagContainer& EffectTags = Spec->CapturedSourceTags.GetSpecTags();
			// FGameplayTagContainer& EffectTags;

			// make a container from array
			FGameplayTagContainer LocalTags = MoveTempIfPossible(EffectTags);
			LocalTags.AppendTags(FGameplayTagContainer::CreateFromArray(Row->Tags));

			LocalTags.AddTagFast(Tag);

			// UE_LOG(LogTemp, Warning, TEXT("HUH Tags %s"), *EffectTags.ToStringSimple());

			// UFPGameplayValueHelpers::MantisGetTagsFromAbilityAndTable();

			// append row tags to the spec
			Spec->AppendDynamicAssetTags(LocalTags);

			// const float BaseValue = Row->GetValueAtLevel(GetScalingLevelForRow(ASC, Row));
			const float BaseValue = Row->GetValueAtLevel(Level);
			// UE_LOG(LogTemp, Warning, TEXT("Eval Level %d %f %s %s"), GetScalingLevelForRow(ASC, Row), BaseValue, *GetScalingTagFromRow(Row).ToString(), *Row->ScalingTag.ToString());

			if (Row->Settings && !Row->Settings->bUseCalculationOnlyForDisplayValue && Row->Settings->ValueCalculation && ASC)
			{
				const float TransformedValue = Row->Settings->ValueCalculation.GetDefaultObject()->Calculate(ASC, BaseValue, LocalTags);
				Spec->SetSetByCallerMagnitude(Tag, TransformedValue);
				// UE_LOG(LogTemp, Warning, TEXT("Found value %s %f"), *Tag.GetTagName().ToString(), TransformedValue);
			}
			else
			{
				Spec->SetSetByCallerMagnitude(Tag, BaseValue);
				// UE_LOG(LogTemp, Warning, TEXT("Found value %s %f"), *Tag.GetTagName().ToString(), BaseValue);
			}
		}
		else
		{
			FMessageLog PIELogger = FMessageLog(FName("PIE"));
			PIELogger.Error(FText::Format(INVTEXT("Failed to find tag from table {0}"), FText::FromName(Tag.GetTagName())));
			UE_LOG(LogTemp, Warning, TEXT("Failed to find tag from table %s"), *Tag.GetTagName().ToString());
		}

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
						if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(PeriodTag.GetTagName(), nullptr, false))
						{
							float Period = Row->GetBaseValue(ASC, Level);

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

FGameplayTag UFPGameplayValueHelpers::TransformScalingTag(FGameplayTag BaseTag)
{
	return BaseTag;
}

FGameplayEffectSpecHandle UFPGameplayValueHelpers::MakeGameplayEffectSpecFromTable(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> GameplayEffectClass, int Level)
{
	if (!ASC)
	{
		return FGameplayEffectSpecHandle();
	}

	UDataTable* Table = UFPGASettings::Get().GetGameValuesTable();
	check(Table);
	if (!Table)
	{
		return FGameplayEffectSpecHandle();
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, Level, ASC->MakeEffectContext());

	UFPGameplayValueHelpers::ApplyGameValueTableToSpec(ASC, SpecHandle, Table);

	return SpecHandle;
}

// FGameplayTag UFPGameplayValueHelpers::GetScalingTagFromRow(const FFPGameplayValueRow* Row)
// {
// 	// TODO
// 	check(false);
// 	return FGameplayTag::EmptyTag;
// 	// return TransformScalingTag(Row->ScalingTags);
// }

// int UFPGameplayValueHelpers::GetScalingLevelForRow(UAbilitySystemComponent* ASC, const FFPGameplayValueRow* Row)
// {
// 	if (!Row)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("Row nullptr"));
// 		return 0;
// 	}
//
// 	// skills start at base level 0
// 	bool bSuccess = false;
// 	const FGameplayAttribute& ScalingAttribute = GetDefault<UFPGASettings>()->SkillLevelAttribute;
//
// 	// in our game we decide the level by the max of lvl of any scaling tags
// 	int Level = 0;
// 	for (const FGameplayTag& Tag : Row->ScalingTags)
// 	{
// 		FGameplayTagContainer TargetTags;
// 		TargetTags.AddTagFast(Tag);
//
// 		Level = FMath::Max(Level, FMath::RoundToInt32(UAbilitySystemBlueprintLibrary::EvaluateAttributeValueWithTags(ASC, ScalingAttribute, FGameplayTagContainer(Tag), TargetTags, bSuccess)));
// 	}
//
// 	return Level;
// }

FGameplayTagContainer UFPGameplayValueHelpers::GatherTagsFromGameplayAbility(const UGameplayAbility* GameplayAbility, FGameplayTag GameValueTag, UDataTable* DataTable)
{
	FGameplayTagContainer Tags;
	if (GameplayAbility->Implements<UFPAbilityInterface>())
	{
		if (const IFPAbilityInterface* AsInterface = Cast<IFPAbilityInterface>(GameplayAbility))
		{
			Tags = AsInterface->GetAbilityTypeTags();
		}
	}

	if (const FFPGameplayValueRow* Row = DataTable->FindRow<FFPGameplayValueRow>(GameValueTag.GetTagName(), nullptr, false))
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
