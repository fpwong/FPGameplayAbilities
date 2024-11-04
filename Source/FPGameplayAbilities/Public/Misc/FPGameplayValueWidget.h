// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FPGameplayValueWidget.generated.h"


class UUniformGridPanel;
class UAbilitySystemComponent;
class UCommonTextBlock;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGameplayValueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void InitWidget(FGameplayTag InTag, UAbilitySystemComponent* InASC);

	virtual void NativePreConstruct() override;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UCommonTextBlock* NameLabel;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UCommonTextBlock* ValueLabel;
};

