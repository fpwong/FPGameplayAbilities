// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilter.h"
#include "FPTargetFilterTypes.h"
#include "FPTargetFilterObserver.generated.h"


USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterObserver
{
	GENERATED_BODY()

	FFPTargetFilter Filter;

	FFPTargetFilterResultChanged OnResultChanged;

	FFPTargetFilterObserver() = default;
	virtual ~FFPTargetFilterObserver()
	{
		// UE_LOG(LogTemp, Warning, TEXT("Observer destructor"));
		Clear();
	}

	void Clear();

	void AddCleanup(const TFunction<void()>& Cleanup)
	{
		CleanupFunctions.Add(Cleanup);
	}

	bool GetCurrentResult() const { return bCurrentResult; }

	virtual void Init(const FFPTargetFilter& FilterTask, AActor* SourceActor, AActor* TargetActor);

	virtual void CheckResultChanged();

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> SourceActorPtr;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActorPtr;

	TArray<TFunction<void()>> CleanupFunctions;

	bool bCurrentResult = false;
};
