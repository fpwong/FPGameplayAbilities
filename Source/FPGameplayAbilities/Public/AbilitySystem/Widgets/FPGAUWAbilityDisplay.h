// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWAbilityDisplay.generated.h"

class UGameplayAbility;
class UProgressBar;
class UCommonTextBlock;

UCLASS(Abstract, meta = (Category = "FPGAButton"))
class FPGAMEPLAYABILITIES_API UFPGAUWAbilityDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* CooldownLabel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UProgressBar* AbilityProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCommonTextBlock* AbilityNameLabel;

	void OnAbilityEnded(UGameplayAbility* GameplayAbility);
	void OnAbilityActivate(UGameplayAbility* GameplayAbility);
	void InitAbility(UAbilitySystemComponent* InAbilitySystem, UGameplayAbility* InAbility);

	void UpdateAbilityProgress();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FGameplayAbilitySpecHandle AbilitySpecHandle;
	UGameplayAbility* Ability;
	UGameplayAbility* ActiveAbility;
	UAbilitySystemComponent* AbilitySystem;
};
