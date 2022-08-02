// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Widgets/FPGAUWAttributeBarWithText.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Widgets/FPGAAttributeBar.h"

void UFPGAUWAttributeBarWithText::BindToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComponent || !CurrentAttribute.IsValid() || !MaxAttribute.IsValid())
	{
		return;
	}

	AbilitySystem = AbilitySystemComponent;

	AttributeBar->BindAndSetAttributes(AbilitySystemComponent, CurrentAttribute, MaxAttribute);

	AttributeDisplayMax->BindAndSetAttributes(AbilitySystemComponent, CurrentAttribute, MaxAttribute);

	if (GainAttributeDisplay)
	{
		GainAttributeDisplay->BindAttribute(AbilitySystemComponent);
	}
}
