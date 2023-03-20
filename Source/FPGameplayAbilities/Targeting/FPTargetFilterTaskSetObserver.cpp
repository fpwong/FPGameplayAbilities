// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterTaskSetObserver.h"

#include "FPTargetFilterPreset.h"
#include "FPTargetFilterTask.h"

void FFPTargetFilterTaskSetObserver::Bind(const FFPTargetFilterTaskSet& InTaskSet, AActor* SourceActor, AActor* TargetActor)
{
	TaskSet = InTaskSet;
	SourceActorPtr = SourceActor;
	TargetActorPtr = TargetActor;

	for (UFPTargetFilterTask* Task : TaskSet.Tasks)
	{
		if (!Task)
		{
			UE_LOG(LogTemp, Warning, TEXT("TaskSet has invalid task!"));
			continue;
		}

		FFPTargetFilterObserver* Binding = Task->MakeBinding(Task, SourceActor, TargetActor);
		if (Binding)
		{
			Binding->OnResultChanged.AddRaw(this, &FFPTargetFilterTaskSetObserver::OnTaskResultChanged);
			Bindings.Add(MakeShareable(Binding));
		}
	}

	bCurrentResult = TaskSet.DoesFilterPass(SourceActor, TargetActor);
}

void FFPTargetFilterTaskSetObserver::CheckResultChanged()
{
	// bool bResult = true;
	// for (TSharedPtr<FFPTargetFilterTaskBinding> Binding : Bindings)
	// {
	// 	if (!Binding->GetCurrentResult())
	// 	{
	// 		bResult = false;
	// 		break;
	// 	}
	// }

	const bool bResult = TaskSet.DoesFilterPass(SourceActorPtr.Get(), TargetActorPtr.Get());

	UE_LOG(LogTemp, Warning, TEXT("Check task set result changed"));
	if (bCurrentResult != bResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("Result changed %d"), bResult);
		bCurrentResult = bResult;
		OnResultChanged.Broadcast(TargetActorPtr.Get(), bResult);
	}
}

