// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterLibrary.h"

#include "FPTargetFilterTaskSet.h"
#include "FPTargetFilterTask_GameplayTag.h"
#include "Kismet/KismetSystemLibrary.h"

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

TArray<AActor*> UFPTargetFilterLibrary::FPTraceActorsCapsule(
	AActor* Source,
	const FVector Start,
	const FVector End,
	const TArray<AActor*>& ActorsToIgnore,
	ETraceTypeQuery TraceChannel,
	float Radius,
	float HalfHeight,
	const FFPTargetFilterTaskSet& FilterTaskSet,
	EDrawDebugTrace::Type DrawDebugType)
{
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::CapsuleTraceMulti(Source, Start, End, Radius, HalfHeight, TraceChannel, false, ActorsToIgnore, DrawDebugType, OutHits, false);

	TArray<AActor*> OutActors;

	for (const FHitResult& OutHit : OutHits)
	{
		if (AActor* Actor = OutHit.GetActor())
		{
			if (FilterTaskSet.DoesFilterPass(Source, Actor))
			{
				OutActors.Add(Actor);
			}
		}
	}

	return OutActors;
}

TArray<AActor*> UFPTargetFilterLibrary::FPTraceActorsSphere(
	AActor* Source,
	const FVector Location,
	const TArray<AActor*>& ActorsToIgnore,
	ETraceTypeQuery TraceChannel,
	float Radius,
	const FFPTargetFilterTaskSet& FilterTaskSet,
	EDrawDebugTrace::Type DrawDebugType)
{
	return FPTraceActorsCapsule(Source, Location, Location, ActorsToIgnore, TraceChannel, Radius, 1.0f, FilterTaskSet, DrawDebugType);
}

TArray<AActor*> UFPTargetFilterLibrary::FilterActorArray(AActor* Source, const TArray<AActor*>& Actors, const FFPTargetFilterTaskSet& FilterTaskSet)
{
	TArray<AActor*> OutActors;
	for (AActor* A : Actors)
	{
		if (FilterTaskSet.DoesFilterPass(Source, A))
		{
			OutActors.Add(A);
		}
	}

	return OutActors;
}
