// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGAProjectile.h"
#include "UObject/Interface.h"
#include "FPGAProjectileInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFPGAProjectileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPGAMEPLAYABILITIES_API IFPGAProjectileInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FTransform GetProjectileSpawn();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	TSubclassOf<AFPGAProjectile> GetProjectileClass();
};
