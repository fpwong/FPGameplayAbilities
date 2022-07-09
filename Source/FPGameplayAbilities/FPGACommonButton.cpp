// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGACommonButton.h"

#include "CommonTextBlock.h"

void UFPGACommonButton::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	ButtonText->SetStyle(GetCurrentTextStyleClass());
}

#if WITH_EDITOR
const FText UFPGACommonButton::GetPaletteCategory()
{
	return FText::FromString("Test UI");
}
#endif
