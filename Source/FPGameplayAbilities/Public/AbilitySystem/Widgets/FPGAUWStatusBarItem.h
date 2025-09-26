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
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> DurationLabel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> DurationBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> NameLabel;

	void SetGameplayEffect(FActiveGameplayEffectHandle InActiveGameplayEffectHandle);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetGameplayEffect(const FActiveGameplayEffectHandle& Effect, const FGameplayTagContainer& EffectTags);

	UFUNCTION(BlueprintCallable)
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	float StartWorldTime = 0.0f;
	float Duration = 0.0f;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo);
};
