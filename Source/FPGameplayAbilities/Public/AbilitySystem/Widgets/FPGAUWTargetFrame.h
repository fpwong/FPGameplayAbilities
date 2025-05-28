﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGAAttributeBar.h"
#include "FPGAUWStatusBar.h"
#include "Blueprint/UserWidget.h"
#include "FPGameplayAbilities/Targeting/FPGAPlayerFocusComponent.h"
#include "FPGAUWTargetFrame.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAUWTargetFrame : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystem = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UFPGAAttributeBar> HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UFPGAUWStatusBar> StatusBar;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> TargetActor;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetActor(AActor* InTargetActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindToTargetingSubsystem();

	UFUNCTION()
	void HandleHoveredActorChanged(AActor* OldActor, AActor* NewActor);

	UFUNCTION()
	void HandleFocusedActorChanged(AActor* OldActor, AActor* NewActor);

	TWeakObjectPtr<AActor> FocusedActor;
	TWeakObjectPtr<AActor> HoveredActor;
};
