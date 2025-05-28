// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "FPGAAttributeDisplay.h"
#include "FPGAAttributeDisplayMax.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWAttributeBarWithText.generated.h"

class UFPGAAttributeBar;
class UAbilitySystemComponent;
UCLASS(Abstract)
class FPGAMEPLAYABILITIES_API UFPGAUWAttributeBarWithText : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void BindToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute CurrentAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute MaxAttribute;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UFPGAAttributeBar> AttributeBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UFPGAAttributeDisplayMax> AttributeDisplayMax;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UFPGAAttributeDisplay> GainAttributeDisplay;

protected:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;
};
