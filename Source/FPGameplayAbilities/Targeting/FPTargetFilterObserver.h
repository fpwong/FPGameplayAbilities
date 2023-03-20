// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTypes.h"
#include "FPTargetFilterObserver.generated.h"


class UFPTargetFilterTask;

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterObserver
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UFPTargetFilterTask* Filter;

	FFPTargetFilterResultChanged OnResultChanged;

	FFPTargetFilterObserver() = default;
	virtual ~FFPTargetFilterObserver() = default;

	bool GetCurrentResult() const { return bCurrentResult; }

	virtual void Init(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor);

	virtual void CheckResultChanged();

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> SourceActorPtr;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActorPtr;

	bool bCurrentResult = false;
};
