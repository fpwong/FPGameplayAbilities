// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FPGAGameState.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API AFPGAGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> CombatLog;

public:
	void UpdateCombatLog(FString Event)
	{
		float Timestamp = GetWorld()->GetTimeSeconds();
		FString EventWithTimestamp = FString::Printf(TEXT("%f: %s"), Timestamp, *Event);
		CombatLog.Add(EventWithTimestamp);
	}
};
