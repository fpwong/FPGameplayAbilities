// Fill out your copyright notice in the Description page of Project Settings.

#include "FPFilteredActorArray.h"

FFPFilteredActorObserver::FFPFilteredActorObserver(const FFPTargetFilterTaskSet& Taskset, AActor* Source, AActor* Target)
{
	OngoingFilterTaskset.Bind(Taskset, Source, Target);
	Actor = Target;
}

void FFPFilteredActorArray::AddActor(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Add %p"), this);
	TSharedRef<FFPFilteredActorObserver> OverlapInstance = MakeShared<FFPFilteredActorObserver>(FilterTaskset, SourceActor.Get(), Actor);
	OverlapInstance->OngoingFilterTaskset.OnResultChanged.AddRaw(this, &FFPFilteredActorArray::OnFilterResultChanged);

	AllActors.Add(Actor, OverlapInstance);

	if (OverlapInstance->OngoingFilterTaskset.GetCurrentResult())
	{
		FilteredActors.Add(Actor);
	}
}

void FFPFilteredActorArray::AddActors(const TArray<AActor*>& Actors)
{
	for (AActor* Actor : Actors)
	{
		AddActor(Actor);
	}
}

void FFPFilteredActorArray::ReplaceWithActors(const TArray<AActor*>& Actors)
{
	ClearActors();
	AddActors(Actors);
}

void FFPFilteredActorArray::ClearActors()
{
	FilteredActors.Empty();
	AllActors.Empty();
}

void FFPFilteredActorArray::RemoveActor(AActor* Actor)
{
	if (AllActors.Contains(Actor))
	{
		if (FilteredActors.Contains(Actor))
		{
			FilteredActors.Remove(Actor);
		}

		AllActors.Remove(Actor);
	}
}

void FFPFilteredActorArray::OnFilterResultChanged(AActor* Target, bool bResult)
{
	if (bResult)
	{
		FilteredActors.Add(Target);
	}
	else
	{
		FilteredActors.Remove(Target);
	}
}

const TArray<AActor*>& UFilteredActorArrayHelper::GetFilteredActors(FFPFilteredActorArray& Array)
{
	return Array.GetFilteredActors();
}

const FFPFilteredActorArray& UFilteredActorArrayHelper::GetFilteredActorsFromMap(const TMap<UObject*, FFPFilteredActorArray>& Map, UObject* Object)
{
	return Map[Object];
}

void UValidActorList::AddActor(AActor* Actor)
{
	if (IsValid(Actor))
	{
		AllActors.Add(Actor);
		Actor->OnDestroyed.AddDynamic(this, &UValidActorList::HandleActorDestroyed);
	}
}

void UValidActorList::RemoveActor(AActor* Actor)
{
	AllActors.Remove(Actor);
	Actor->OnDestroyed.RemoveAll(this);
}

void UValidActorList::SetActors(const TArray<AActor*>& Actors)
{
	ClearActors();
	for (AActor* Actor : Actors)
	{
		AddActor(Actor);
	}
}

void UValidActorList::ClearActors()
{
	for (AActor* Actor : AllActors)
	{
		RemoveActor(Actor);
	}
}

void UValidActorList::HandleActorDestroyed(AActor* Actor)
{
	AllActors.Remove(Actor);
}
