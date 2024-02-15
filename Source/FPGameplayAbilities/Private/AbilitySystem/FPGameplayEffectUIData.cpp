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

FText UFPGameplayUIHelpers::GenerateGameplayEffectModifiersDescription(TSubclassOf<UGameplayEffect> GameplayEffectClass, UDataTable* ValueTable, const TMap<FGameplayAttribute, FFPUIData>& AttributeUIDataMapping)
{
	if (!GameplayEffectClass)
	{
		return INVTEXT("No Gameplay Effect");
	}

	TArray<FText> Lines;

	UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
		if (!GameplayEffect)
    	{
    		return INVTEXT("No Gameplay Effect");
    	}

	for (const FGameplayModifierInfo& Modifier : GameplayEffect->Modifiers)
	{
		FFormatOrderedArguments Args;

		// Modifier.Attribute;

		FString ValueString;
		const auto& Tag = Modifier.ModifierMagnitude.GetSetByCallerFloat().DataTag;
		if (Tag.IsValid())
		{
			UFPGameplayValueHelpers::GetDisplayValueFromTable(ValueTable, Tag, ValueString);
		}
		else
		{
			float OutValue;
			if (Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(0, OutValue))
			{
				FString PrefixSign = "";
				if (OutValue != 0)
				{
					PrefixSign = OutValue > 0 ? "+" : "-"; 
				}
				ValueString = PrefixSign + FString::SanitizeFloat(OutValue, 0);
			}
		}

		// value
		Args.Add(FText::FromString(ValueString));

		// tags
		FText Tags;
		if (!Modifier.SourceTags.IsEmpty())
		{
			TArray<FString> TagsNames;
			for (const FGameplayTag& RequiredTag : Modifier.SourceTags.RequireTags.GetGameplayTagArray())
			{
				TagsNames.Add(UFPGAGameplayAbilitiesLibrary::GetSimpleTagName(RequiredTag).ToString());
			}

			Tags = FText::FromString(FString::Join(TagsNames, TEXT(" ")) + " ");
		}
		Args.Add(Tags);

		// attribute
		if (const FFPUIData* Data = AttributeUIDataMapping.Find(Modifier.Attribute))
		{
			Args.Add(Data->Name);
		}
		else
		{
			Args.Add(FText::FromString(Modifier.Attribute.AttributeName));
		}

		if (!ValueString.IsEmpty())
		{

			FText Line = FText::Format(INVTEXT("{0} to {1}{2}"), Args);
			Lines.Add(Line);
		}
	}

	return FText::Join(INVTEXT("\n"), Lines);
}
