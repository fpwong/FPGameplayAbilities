// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWStatusBarItem.generated.h"

class UProgressBar;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWStatusBarItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UAbilitySystemComponent* AbilitySystem;

	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCommonTextBlock* DurationLabel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UProgressBar* DurationBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCommonTextBlock* NameLabel;

	void SetGameplayEffect(FActiveGameplayEffectHandle InActiveGameplayEffectHandle);

protected:
	float StartWorldTime = 0.0f;
	float Duration = 0.0f;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo);
};
