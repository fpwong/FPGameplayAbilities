// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterObserver.h"
#include "FPTargetFilterPreset.h"
#include "FPTargetFilterTaskSetObserver.generated.h"

// class UFPTargetFilterPreset;

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTaskSetObserver
{
	GENERATED_BODY()

public:
	FFPTargetFilterTaskSetObserver() = default;

	FFPTargetFilterResultChanged OnResultChanged;

	// UPROPERTY()
	FFPTargetFilterTaskSet TaskSet;

	bool GetCurrentResult() const { return bCurrentResult; }

	void CheckResultChanged();

	void Bind(const FFPTargetFilterTaskSet& InTaskSet, AActor* SourceActor, AActor* TargetActor);

private:
	void OnTaskResultChanged(AActor*, bool) { CheckResultChanged(); }

	bool bCurrentResult = false;

	TArray<TSharedPtr<FFPTargetFilterObserver>> Bindings;
	
	TArray<TSharedPtr<FFPTargetFilterTaskSetObserver>> TaskSetBindings;

	UPROPERTY()
	TWeakObjectPtr<AActor> SourceActorPtr;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActorPtr;
};
