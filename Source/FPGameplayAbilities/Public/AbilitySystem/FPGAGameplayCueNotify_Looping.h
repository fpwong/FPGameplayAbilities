// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Looping.h"
#include "FPGAGameplayCueNotify_Looping.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API AFPGAGameplayCueNotify_Looping : public AGameplayCueNotify_Looping
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (InlineEditConditionToggle))
	uint32 bUseRelativeHeightOffset : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (EditCondition = "bUseRelativeHeightOffset"))
	float RelativeHeightOffset = 1.0f;

	FGameplayCueParameters MakeModifiedParameters(AActor* Target, const FGameplayCueParameters& Parameters);

	virtual bool OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	virtual bool WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
};
