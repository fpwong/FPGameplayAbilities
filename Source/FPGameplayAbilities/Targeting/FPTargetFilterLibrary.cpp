// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterLibrary.h"

#include "FPTargetFilterTaskSet.h"
#include "FPTargetFilterTask_GameplayTag.h"

void UFPTargetFilterLibrary::GetAllRequiredTagsInTaskSet(const FFPTargetFilterTaskSet& TaskSet, AActor* Source, AActor* Target, const FGameplayTagContainer& Tags, FGameplayTagContainer& OUT OutTags)
{
	TaskSet.IterateTasks(Source, Target, [&Tags = Tags, &OutTags = OutTags](UFPTargetFilterTask* Task, bool bResult)
	{
		if (Task->TaskType == EFPTargetFilterTaskType::FailureTask)
		{
			if (UFPTargetFilterTask_GameplayTag* GameplayTagFilter = Cast<UFPTargetFilterTask_GameplayTag>(Task))
			{
				OutTags.AppendTags(GameplayTagFilter->TargetRequirements.RequireTags.Filter(Tags));
			}
		}
	});
	// for (UFPTargetFilterTask* Filter : TaskSet.Tasks)
	// {
	// 	if (auto GameplayFilter = Cast<UFPTargetFilterTask_GameplayTag>(Filter))
	// 	{
	// 		
	// 	}
	//
	// 	if (!Filter->ChildTaskSet.IsEmpty())
	// 	{
	// 		QueryForTags(Filter->ChildTaskSet, Tags, OutTags);
	// 	}
	// }
}
