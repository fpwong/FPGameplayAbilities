// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "FPGAUWAttributeList.generated.h"

class UAbilitySystemComponent;
class UFPGAAttributeDisplay;
class UFPGAAttributeDisplayMax;

USTRUCT(BlueprintType)
struct FFPGAAttributePair
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute MinAttribute;

	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute MaxAttribute;
};

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWAttributeList : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGAAttributeDisplay> AttributeDisplayClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGAAttributeDisplayMax> AttributeDisplayMaxClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<FFPGAAttributePair> AttributesToShow;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPanelWidget> ListPanel;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void BindToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent);
};
