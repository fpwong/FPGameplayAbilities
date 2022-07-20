// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWStatusBar.generated.h"

class UFPGAUWStatusBarItem;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWStatusBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayEffectQuery EffectQuery;

	UPROPERTY(VisibleAnywhere)
	UAbilitySystemComponent* BoundAbilitySystem;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPanelWidget* StatusBarPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGAUWStatusBarItem> StatusBarItemClass;

	UFUNCTION(BlueprintCallable)
	void SetActor(AActor* Actor);

protected:
	void CreateStatusBarItem(const FActiveGameplayEffect* ActiveGameplayEffect);
	void HandleGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
