// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWAttributeList.h"

#include "AbilitySystem/Widgets/FPGAAttributeDisplay.h"
#include "AbilitySystem/Widgets/FPGAAttributeDisplayMax.h"
#include "Blueprint/WidgetTree.h"

void UFPGAUWAttributeList::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFPGAUWAttributeList::BindToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent)
{
	ListPanel->ClearChildren();
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!AttributeDisplayMaxClass || !AttributeDisplayClass)
	{
		return;
	}

	for (FFPGAAttributePair& AttributePair : AttributesToShow)
	{
		if (!AttributePair.MinAttribute.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("UFPGAUWAttributeList: Invalid min attribute!"));
			continue;
		}

		// UE_LOG(LogTemp, Warning, TEXT("Show attribute list?"));
		// if we have a max attribute, spawn an attribute display max
		if (AttributePair.MaxAttribute.IsValid())
		{
			// UE_LOG(LogTemp, Warning, TEXT("Spawn item max?"));
			UFPGAAttributeDisplayMax* AttributeDisplayMax = WidgetTree->ConstructWidget<UFPGAAttributeDisplayMax>(AttributeDisplayMaxClass);
			AttributeDisplayMax->BindAndSetAttributes(AbilitySystemComponent, AttributePair.MinAttribute, AttributePair.MaxAttribute);
			ListPanel->AddChild(AttributeDisplayMax);
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("Spawn item min?"));
			auto AttributeDisplay = WidgetTree->ConstructWidget<UFPGAAttributeDisplay>(AttributeDisplayClass);
			AttributeDisplay->BindAndSetAttribute(AbilitySystemComponent, AttributePair.MinAttribute);
			ListPanel->AddChild(AttributeDisplay);
		}
	}
}
