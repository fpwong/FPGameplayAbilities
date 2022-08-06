// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FPGAGameplayAbilityInterface.generated.h"

struct FGameplayTag;
class UGameplayAbility;

UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPGAGameplayAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class FPGAMEPLAYABILITIES_API IFPGAGameplayAbilityInterface
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UGameplayAbility> GetAbilityByTag(FGameplayTag GameplayTag) = 0; 
};
