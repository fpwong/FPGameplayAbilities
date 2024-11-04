﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayValueWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FPGameplayValueListWidget.generated.h"

class UAbilitySystemComponent;
class UUniformGridPanel;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGameplayValueWidgetList : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UUniformGridPanel* MainPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGameplayValueWidget> GameplayValueWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer DebugTags;

	UFUNCTION(BlueprintNativeEvent)
	void InitWidgets(FGameplayTagContainer InTags, UAbilitySystemComponent* InASC);

	virtual void NativePreConstruct() override;
};