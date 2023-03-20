// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterObserver.h"

#include "FPTargetFilterTask.h"

void FFPTargetFilterObserver::Init(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	Filter = FilterTask;
	SourceActorPtr = SourceActor;
	TargetActorPtr = TargetActor;

	bCurrentResult = Filter->DoesFilterPass(SourceActorPtr.Get(), TargetActorPtr.Get());
}

void FFPTargetFilterObserver::CheckResultChanged()
{
	const bool bNewResult = Filter->DoesFilterPass(SourceActorPtr.Get(), TargetActorPtr.Get());
	if (bCurrentResult != bNewResult)
	{
		bCurrentResult = bNewResult;
		OnResultChanged.Broadcast(TargetActorPtr.Get(), bNewResult);
	}
}
