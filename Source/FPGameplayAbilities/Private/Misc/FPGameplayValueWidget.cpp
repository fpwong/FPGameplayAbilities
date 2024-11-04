// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/FPGameplayValueWidget.h"

#include "CommonTextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "FPGameplayAbilities/FPGASettings.h"
#include "Misc/FPGameplayValueRow.h"

void UFPGameplayValueWidget::InitWidget_Implementation(FGameplayTag InTag, UAbilitySystemComponent* InASC)
{
	Tag = InTag;

	UDataTable* Table = UFPGASettings::GetMutable().GetGameValuesTable();
	if (!Table)
	{
		return;
	}

	// set value string
	{
		FString ValueAsStr;
		UFPGameplayValueHelpers::GetDisplayValueFromTable(Table, InASC, InTag, ValueAsStr, 1);

		if (ValueLabel)
		{
			ValueLabel->SetText(FText::FromString(ValueAsStr));
		}
	}

	// set name string
	if (const FFPGameplayValueRow* Row = Table->FindRow<FFPGameplayValueRow>(Tag.GetTagName(), nullptr, false))
	{
		if (NameLabel)
		{
			NameLabel->SetText(FText::FromString(Row->ValueName));
		}
	}
}

void UFPGameplayValueWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime() && Tag.IsValid())
	{
		InitWidget(Tag, nullptr);
	}
}
