// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilter.h"

bool FFPTargetFilterTask_Compound::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, FGameplayTagContainer* OutFailureTags) const
{
	for (const FInstancedStruct& InstStruct : Tasks)
	{
		const FFPTargetFilterTask& Task = InstStruct.Get<FFPTargetFilterTask>();
		switch (Task.TaskType)
		{
		case EFPTargetFilterTaskType::FailureTask:
			if (!Task.DoesFilterPass(SourceActor, TargetActor, OutFailureTags))
			{
				return false;
			}
			break;
		case EFPTargetFilterTaskType::PassingTask:
			if (Task.DoesFilterPass(SourceActor, TargetActor))
			{
				return true;
			}
		// case EFPTargetFilterTaskType::ConditionalTask:
			// if (Task.DoesFilterPass(SourceActor, TargetActor))
			// {
			// 	return Task.ChildTaskSet.DoesFilterPass(SourceActor, TargetActor, OutFailureTags);
			// }
			// break;
		// default: ;
		}
	}

	return true;
}

void FFPTargetFilterTask_Compound::BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const
{
	for (const FInstancedStruct& Task : Tasks)
	{
		if (Task.IsValid())
		{
			Task.Get<FFPTargetFilterTask>().BindToChanges(Observer, SourceActor, TargetActor);
		}
	}

}

FFPTargetFilter::FFPTargetFilter(const FFPTargetFilterTask& InTask)
{
	Task = FInstancedStruct::Make(InTask);
}

bool FFPTargetFilter::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, FGameplayTagContainer* OutFailureTags) const
{
	const FFPTargetFilterTask& TaskStruct = GetTask();
	switch (TaskStruct.TaskType)
	{
	case EFPTargetFilterTaskType::FailureTask:
		if (!TaskStruct.DoesFilterPass(SourceActor, TargetActor, OutFailureTags))
		{
			return false;
		}
		break;
	case EFPTargetFilterTaskType::PassingTask:
		if (TaskStruct.DoesFilterPass(SourceActor, TargetActor))
		{
			return true;
		}
	// case EFPTargetFilterTaskType::ConditionalTask:
		// if (TaskStruct.DoesFilterPass(SourceActor, TargetActor))
		// {
		// 	// return TaskStruct.ChildTaskSet.DoesFilterPass(SourceActor, TargetActor, OutFailureTags);
		// }
		// break;
	// default: ;
	}

	return true;
}

const FFPTargetFilterTask& FFPTargetFilter::GetTask() const
{
	static FFPTargetFilterTask InvalidTask;
	return Task.IsValid() ? Task.Get<FFPTargetFilterTask>() : InvalidTask;
}

TArray<FInstancedStruct> FFPTargetFilter::GetAllTasks()
{
	TArray<FInstancedStruct> AllTasks;
	TArray<FInstancedStruct> Stack = { Task };

	while (!Stack.IsEmpty())
	{
		const FInstancedStruct& Next = Stack.Pop();
		AllTasks.Add(Next);

		const FFPTargetFilterTask& NextTask = Next.Get<FFPTargetFilterTask>();
		NextTask.GetChildTasks(Stack);
	}

	return AllTasks;
}

bool FFPTargetFilter::IsValid() const
{
	return Task.IsValid();
}
