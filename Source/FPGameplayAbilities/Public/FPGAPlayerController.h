// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPGAPlayerController.generated.h"

UCLASS()
	class FPGAMEPLAYABILITIES_API AFPGAPlayerController
	: public APlayerController
{
	GENERATED_BODY()

public:
	AFPGAPlayerController();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "PlayerController")
	bool GetHitResultsUnderCursor(const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, TArray<FHitResult>& OutHits);
};
