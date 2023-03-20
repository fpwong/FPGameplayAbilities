// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTaskSet.h"
#include "FPTargetFilterTypes.h"
#include "UObject/Object.h"
#include "FPTargetFilterTask.generated.h"

struct FFPTargetFilterObserver;

UCLASS(EditInlineNew, BlueprintType)
class FPGAMEPLAYABILITIES_API UFPTargetFilterTask : public UObject
{
	GENERATED_BODY()

public:
	UFPTargetFilterTask();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	EFPTargetFilterTaskType TaskType = EFPTargetFilterTaskType::FailureTask;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default, meta = (
		EditCondition = "TaskType == EFPTargetFilterTaskType::ConditionalTask",
		EditConditionHides,
		ShowOnlyInnerProperties))
	FFPTargetFilterTaskSet ChildTaskSet;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const { return false; }

	virtual FFPTargetFilterObserver* MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) { return nullptr; }
};