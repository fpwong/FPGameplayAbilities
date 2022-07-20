// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGAAttributeBar.h"
#include "FPGAUWStatusBar.h"
#include "Blueprint/UserWidget.h"
#include "FPGAUWTargetFrame.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWTargetFrame : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	AActor* TargetActor;

	UPROPERTY(BlueprintReadOnly)
	UAbilitySystemComponent* TargetAbilitySystem = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFPGAAttributeBar* HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFPGAUWStatusBar* StatusBar;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetActor(AActor* InTargetActor);
};
