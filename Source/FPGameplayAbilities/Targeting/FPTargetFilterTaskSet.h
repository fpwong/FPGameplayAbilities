// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTaskSet.generated.h"

class UFPTargetFilterTask;
struct FGameplayTagContainer;

USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTaskSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category=Default)
	TArray<TObjectPtr<UFPTargetFilterTask>> Tasks;

	bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const;

	bool IterateTasks(const AActor* SourceActor, const AActor* TargetActor, TFunctionRef<void(UFPTargetFilterTask* Task, bool bResult)> Func) const;

	bool IsEmpty() const { return Tasks.Num() == 0; }
};
