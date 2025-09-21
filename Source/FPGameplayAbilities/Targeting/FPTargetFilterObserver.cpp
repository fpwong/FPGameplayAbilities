// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterObserver.h"

void FFPTargetFilterObserver::Clear()
{
	// UE_LOG(LogTemp, Warning, TEXT("Observer cleanup!"))
	for (const TFunction<void()>& Cleanup : CleanupFunctions)
	{
		Cleanup();
	}

	CleanupFunctions.Empty();
}

void FFPTargetFilterObserver::Init(const FFPTargetFilter& FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	if (!ensure(FilterTask.IsValid()))
	{
		return;
	}

	Filter = FilterTask;
	SourceActorPtr = SourceActor;
	TargetActorPtr = TargetActor;

	bCurrentResult = Filter.GetTask().DoesFilterPass(SourceActorPtr.Get(), TargetActorPtr.Get());
	// UE_LOG(LogTemp, Warning, TEXT("Initial result %d"), bCurrentResult);

	Filter.GetTask().BindToChanges(*this, SourceActor, TargetActor);
}

void FFPTargetFilterObserver::CheckResultChanged()
{
	const bool bNewResult = Filter.GetTask().DoesFilterPass(SourceActorPtr.Get(), TargetActorPtr.Get());
	if (bCurrentResult != bNewResult)
	{
		bCurrentResult = bNewResult;
		OnResultChanged.Broadcast(TargetActorPtr.Get(), bNewResult);

		// UE_LOG(LogTemp, Warning, TEXT("Checking result changed %d"), bCurrentResult);
	}
}
