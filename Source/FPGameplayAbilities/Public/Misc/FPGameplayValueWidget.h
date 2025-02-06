// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "FPGameplayValueWidget.generated.h"


class UUniformGridPanel;
class UCommonTextBlock;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGameplayValueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void InitWidget(FGameplayTag InTag, UAbilitySystemComponent* InASC, int InLevel);

	virtual void NativePreConstruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateLevel(int NewLevel);

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;

	UPROPERTY(Transient)
	int Level;

	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UCommonTextBlock* NameLabel;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UCommonTextBlock* ValueLabel;
};

