// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"

#include "FPGAUWDamageNumber.generated.h"

class UWidgetComponent;
class UFPGADamageNumberComponent;
class UCommonTextBlock;
class UFPGAUWDamageNumber;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPGAOnDamageNumberEnded, UFPGAUWDamageNumber*, DamageNumber);

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWDamageNumber : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FFPGAOnDamageNumberEnded OnDamageNumberEnded;

	UPROPERTY(BlueprintReadOnly)
	UWidgetComponent* OwnerWidgetComponent;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* DamageNumberLabel;

public:
	void InitDamageNumber(UWidgetComponent* InOwnerWidgetComponent, float Damage, const FGameplayTagContainer& DamageTags);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="InitDamageNumber"))
	void BP_InitDamageNumber(UWidgetComponent* InOwnerWidgetComponent, float Damage, const FGameplayTagContainer& DamageTags);

	UFUNCTION(BlueprintCallable)
	void EndDamageNumber();
};
