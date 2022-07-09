// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Targeting/FPGATargetFilter.h"
#include "FPGADeveloperSettings.generated.h"

UCLASS(Config="Game", defaultconfig, meta=(DisplayName="FPGameplayAbilities"))
class FPGAMEPLAYABILITIES_API UFPGADeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Targeting", EditAnywhere)
	FFPGATargetFilter DefaultStageTargetFilter;
};
