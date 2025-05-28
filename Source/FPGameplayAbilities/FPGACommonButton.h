// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "FPGACommonButton.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGACommonButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnCurrentTextStyleChanged() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> ButtonText;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
};
