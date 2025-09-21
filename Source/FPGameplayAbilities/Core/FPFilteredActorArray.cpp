// TODO convert to use new target filter
// // Fill out your copyright notice in the Description page of Project Settings.
//
// #include "FPFilteredActorArray.h"
//
// FFPFilteredActorObserver::FFPFilteredActorObserver(const FFPTargetFilterTaskSet& Taskset, AActor* Source, AActor* Target)
// {
// 	OngoingFilterTaskset.Bind(Taskset, Source, Target);
// 	Actor = Target;
// }
//
// void FFPFilteredActorArray::AddActor(AActor* Actor)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Add %p"), this);
// 	TSharedRef<FFPFilteredActorObserver> OverlapInstance = MakeShared<FFPFilteredActorObserver>(FilterTaskset, SourceActor.Get(), Actor);
// 	OverlapInstance->OngoingFilterTaskset.OnResultChanged.AddRaw(this, &FFPFilteredActorArray::OnFilterResultChanged);
//
// 	AllActors.Add(Actor, OverlapInstance);
//
// 	if (OverlapInstance->OngoingFilterTaskset.GetCurrentResult())
// 	{
// 		FilteredActors.Add(Actor);
// 	}
// }
//
// void FFPFilteredActorArray::AddActors(const TArray<AActor*>& Actors)
// {
// 	for (AActor* Actor : Actors)
// 	{
// 		AddActor(Actor);
// 	}
// }
//
// void FFPFilteredActorArray::ReplaceWithActors(const TArray<AActor*>& Actors)
// {
// 	ClearActors();
// 	AddActors(Actors);
// }
//
// void FFPFilteredActorArray::ClearActors()
// {
// 	FilteredActors.Empty();
// 	AllActors.Empty();
// }
//
// void FFPFilteredActorArray::RemoveActor(AActor* Actor)
// {
// 	if (AllActors.Contains(Actor))
// 	{
// 		if (FilteredActors.Contains(Actor))
// 		{
// 			FilteredActors.Remove(Actor);
// 		}
//
// 		AllActors.Remove(Actor);
// 	}
// }
//
// void FFPFilteredActorArray::OnFilterResultChanged(AActor* Target, bool bResult)
// {
// 	if (bResult)
// 	{
// 		FilteredActors.Add(Target);
// 	}
// 	else
// 	{
// 		FilteredActors.Remove(Target);
// 	}
// }
//
// const TArray<AActor*>& UFilteredActorArrayHelper::GetFilteredActors(FFPFilteredActorArray& Array)
// {
// 	return Array.GetFilteredActors();
// }