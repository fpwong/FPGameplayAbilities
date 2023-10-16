// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/EngineSubsystem.h"
#include "FPTokenSubsystem.generated.h"

struct FGameplayAttribute;
class UGameplayEffect;

// TODO test if this works for a networked game
UCLASS()
class FPGAMEPLAYABILITIES_API UFPTokenSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static UFPTokenSubsystem& Get();

	UFUNCTION(BlueprintCallable)
	UGameplayEffect* FindOrAddTokenEffect(FGameplayTag Tag, FGameplayAttribute Attribute);

	UPROPERTY()
	TMap<FGameplayTag, UGameplayEffect*> TokenMap;
};
