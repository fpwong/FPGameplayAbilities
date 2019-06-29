// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FPGAAIController.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API AFPGAAIController 
	: public AAIController
{
	GENERATED_BODY()

	AFPGAAIController();

public:
	FGenericTeamId GetGenericTeamId() const override;

	void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
};
