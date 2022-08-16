// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "FPGAGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:
	virtual void FlushPendingCues() override;
};
