// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTaskSet.generated.h"

class UFPTargetFilterTask;

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTaskSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category="TargetFilterPreset")
	TArray<TObjectPtr<UFPTargetFilterTask>> Tasks;

	bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const;

	bool IsEmpty() const { return Tasks.Num() == 0; }
};