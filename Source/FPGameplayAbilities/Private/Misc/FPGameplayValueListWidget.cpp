// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/FPGameplayValueListWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Misc/FPGameplayValueWidget.h"

void UFPGameplayValueWidgetList::InitWidgets_Implementation(FGameplayTagContainer InTags, UAbilitySystemComponent* InASC)
{
	if (MainPanel)
	{
		MainPanel->ClearChildren();
		for (int i = 0; i < InTags.Num(); ++i)
		{
			auto Tag = InTags.GetByIndex(i);
			if (UFPGameplayValueWidget* NewWidget = WidgetTree->ConstructWidget<UFPGameplayValueWidget>(GameplayValueWidgetClass))
			{
				UUniformGridSlot* NewSlot = MainPanel->AddChildToUniformGrid(NewWidget, i, 0);
				NewSlot->SetHorizontalAlignment(HAlign_Fill);
				NewWidget->InitWidget(Tag, InASC);
			}
		}
	}
}

void UFPGameplayValueWidgetList::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (IsDesignTime())
	{
		InitWidgets(DebugTags, nullptr);
	}
}
