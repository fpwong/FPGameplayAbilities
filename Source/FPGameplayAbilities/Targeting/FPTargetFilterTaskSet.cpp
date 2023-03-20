// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterTaskSet.h"

#include "FPTargetFilterTask.h"
#include "FPTargetFilterTypes.h"

bool FFPTargetFilterTaskSet::DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
{
	for (auto Task : Tasks)
	{
		if (!Task)
		{
			continue;
		}

		const bool bFilterResult = Task->DoesFilterPass(SourceActor, TargetActor);

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
