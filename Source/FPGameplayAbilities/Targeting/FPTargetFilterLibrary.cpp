// Fill out your copyright notice in the Description page of Project Settings.

#include "FPTargetFilterLibrary.h"
#include "FPTargetFilter.h"
#include "FPTargetFilterPreset.h"
#include "FPTargetFilterTask_GameplayTag.h"
#include "FPTargetFilterTask_AttributeComparison.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<AActor*> UFPTargetFilterLibrary::FPTraceActorsCapsule(
	AActor* Source,
	const FVector Start,
	const FVector End,
	const TArray<AActor*>& ActorsToIgnore,
	ETraceTypeQuery TraceChannel,
	float Radius,
	float HalfHeight,
	const FFPTargetFilter& FilterTaskSet,
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
	const FFPTargetFilter& FilterTaskSet,
	EDrawDebugTrace::Type DrawDebugType)
{
	return FPTraceActorsCapsule(Source, Location, Location, ActorsToIgnore, TraceChannel, Radius, 1.0f, FilterTaskSet, DrawDebugType);
}

TArray<AActor*> UFPTargetFilterLibrary::FilterActorArray(AActor* Source, const TArray<AActor*>& Actors, const FFPTargetFilter& FilterTaskSet)
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

FFPTargetFilter UFPTargetFilterLibrary::Conv_TaskGameplayTag(const FFPTargetFilterTask_GameplayTag& FilterTask)
{
	FFPTargetFilter Filter;
	Filter.Task = FInstancedStruct::Make(FilterTask);
	return Filter;
}

FFPTargetFilter UFPTargetFilterLibrary::Conv_TaskRelationship(const FFPTargetFilterTask_Relationship& FilterTask)
{
	return FFPTargetFilter(FilterTask); 
	// FFPTargetFilter Filter;
	// Filter.Task = FInstancedStruct::Make(FilterTask);
	// return Filter;
}

FFPTargetFilter UFPTargetFilterLibrary::Conv_TaskAttributeComparison(const FFPTargetFilterTask_AttributeComparison& FilterTask)
{
	FFPTargetFilter Filter;
	Filter.Task = FInstancedStruct::Make(FilterTask);
	return Filter;
}

FFPTargetFilter UFPTargetFilterLibrary::Conv_TaskPreset(const FFPTargetFilterTask_Preset& FilterTask)
{
	FFPTargetFilter Filter(FilterTask);
	Filter.Task = FInstancedStruct::Make(FilterTask);
	return Filter;
}

bool UFPTargetFilterLibrary::EvaluateFilterTask(const FFPTargetFilter& FilterTask, AActor* SourceActor, AActor* TargetActor)
{
	return FilterTask.DoesFilterPass(SourceActor, TargetActor, nullptr);
}
