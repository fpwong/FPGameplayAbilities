// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterTaskSet.h"

#include "FPTargetFilterTask.h"
#include "FPTargetFilterTypes.h"

bool FFPTargetFilterTaskSet::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags) const
{
	for (auto Task : Tasks)
	{
		if (!Task)
		{
			continue;
		}

		switch (Task->TaskType)
		{
			case EFPTargetFilterTaskType::FailureTask:
				if (!Task->DoesFilterPass(SourceActor, TargetActor, OutFailureTags))
				{
					return false;
				}
			break;
			case EFPTargetFilterTaskType::PassingTask:
				if (Task->DoesFilterPass(SourceActor, TargetActor))
				{
					return true;
				}
			case EFPTargetFilterTaskType::ConditionalTask:
				if (Task->DoesFilterPass(SourceActor, TargetActor))
				{
					return Task->ChildTaskSet.DoesFilterPass(SourceActor, TargetActor, OutFailureTags);
				}
			break;
			default: ;
		}
	}

	return true;
}

bool FFPTargetFilterTaskSet::IterateTasks(const AActor* SourceActor, const AActor* TargetActor, TFunctionRef<void(UFPTargetFilterTask* Task, bool bResult)> Func) const
{
	for (auto Task : Tasks)
	{
		if (!Task)
		{
			continue;
		}

		const bool bFilterResult = Task->DoesFilterPass(SourceActor, TargetActor);

		Func(Task, bFilterResult);

		switch (Task->TaskType)
		{
			case EFPTargetFilterTaskType::FailureTask:
				if (!bFilterResult)
				{
					return false;
				}
			break;
			case EFPTargetFilterTaskType::PassingTask:
				if (bFilterResult)
				{
					return true;
				}
			case EFPTargetFilterTaskType::ConditionalTask:
				if (bFilterResult)
				{
					return Task->ChildTaskSet.DoesFilterPass(SourceActor, TargetActor);
				}
			break;
			default: ;
		}
	}

	return true;
}
