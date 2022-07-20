// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWStatusBarItem.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWStatusBarItem : public UUserWidget
{
	GENERATED_BODY()

public:
	const UAbilitySystemComponent* AbilitySystem;

	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* DurationLabel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCommonTextBlock* NameLabel;

	void SetGameplayEffect(const FActiveGameplayEffectHandle& InActiveGameplayEffectHandle);

	const FActiveGameplayEffect* GetActiveGameplayEffect();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
