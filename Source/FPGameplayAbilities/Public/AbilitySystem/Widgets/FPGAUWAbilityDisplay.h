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
	UPROPERTY(EditDefaultsOnly)
	bool bFullProgressWhenOnCooldown = true;

	UPROPERTY(EditDefaultsOnly)
	bool bInvertProgress = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCommonTextBlock* CooldownLabel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UProgressBar* AbilityProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UCommonTextBlock* AbilityNameLabel;

	void HandleAbilityEnded(UGameplayAbility* GameplayAbility);
	void HandleAbilityActivate(UGameplayAbility* GameplayAbility);
	void HandleAbilityCommitted(UGameplayAbility* GameplayAbility);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPAbilityDisplayEvent, UFPGAUWAbilityDisplay*, AbilityDisplay);

	UPROPERTY(BlueprintAssignable)
	FFPAbilityDisplayEvent OnAbilityActivated;

	UPROPERTY(BlueprintAssignable)
	FFPAbilityDisplayEvent OnAbilityCommited;

	UPROPERTY(BlueprintAssignable)
	FFPAbilityDisplayEvent OnAbilityEnded;

	UFUNCTION(BlueprintCallable)
	void InitAbility(UAbilitySystemComponent* InAbilitySystem, UGameplayAbility* InAbility);

	void UpdateAbilityProgress();

	void ClearAbility();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FGameplayAbilitySpecHandle AbilitySpecHandle;

	UPROPERTY()
	UGameplayAbility* Ability;

	UPROPERTY()
	UGameplayAbility* ActiveAbility;

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystem;
};
