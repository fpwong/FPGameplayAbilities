// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAOverlapManager.h"

FFPGAOverlapInstance::FFPGAOverlapInstance(const FFPTargetFilterTaskSet& Taskset, AActor* Source, AActor* Target)
{
	OngoingFilterTaskset.Bind(Taskset, Source, Target);
	Actor = Target;
}

UFPGAOverlapManager::UFPGAOverlapManager(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFPGAOverlapManager::BeginPlay()
{
	Super::BeginPlay();

	AActor* OwnerActor = GetOwner();
	SourceActorPtr = OwnerActor;

	if (bBindToOwner)
	{
		BindToActor(OwnerActor);
	}
}

TArray<AActor*> UFPGAOverlapManager::GetAllOverlappingActors() const
{
	TArray<AActor*> OutActors;
	for (auto& Elem : AllOverlappingActors)
	{
		TWeakObjectPtr<AActor> OverlappedActor = Elem.Key;
		if (OverlappedActor.IsValid())
		{
			OutActors.Add(OverlappedActor.Get());
		}
	}

	return OutActors;
}

void UFPGAOverlapManager::SetSourceActor(AActor* Actor)
{
	SourceActorPtr = Actor;
}

void UFPGAOverlapManager::BindToPrimitiveComponent(UPrimitiveComponent* Component)
{
	if (!Component)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPGAOverlapManager::BindToPrimitiveComponent: Component is null"));
		return;
	}

	TArray<AActor*> InitialOverlappingActors;
	Component->GetOverlappingActors(InitialOverlappingActors);

	for (AActor* InitialOverlapping : InitialOverlappingActors)
	{
		HandleBeginOverlap(InitialOverlapping);
	}

	Component->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPrimitiveBeginOverlap);
	Component->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnPrimitiveEndOverlap);
}

void UFPGAOverlapManager::BindToActor(AActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPGAOverlapManager::BindToActor: Actor is null"));
		return;
	}

	TArray<AActor*> InitialOverlappingActors;
	Actor->GetOverlappingActors(InitialOverlappingActors);

	for (AActor* InitialOverlapping : InitialOverlappingActors)
	{
		HandleBeginOverlap(InitialOverlapping);
	}

	Actor->OnActorBeginOverlap.AddDynamic(this, &UFPGAOverlapManager::OnActorBeginOverlap);
	Actor->OnActorEndOverlap.AddDynamic(this, &UFPGAOverlapManager::OnActorEndOverlap);
}

void UFPGAOverlapManager::HandleBeginOverlap(AActor* OtherActor)
{
	if (SourceActorPtr.IsValid())
	{
		TSharedRef<FFPGAOverlapInstance> OverlapInstance = MakeShared<FFPGAOverlapInstance>(FilterTaskset, SourceActorPtr.Get(), OtherActor);
		OverlapInstance->OngoingFilterTaskset.OnResultChanged.AddUObject(this, &ThisClass::OnFilterResultChanged);

		AllOverlappingActors.Add(OtherActor, OverlapInstance);

		if (OverlapInstance->OngoingFilterTaskset.GetCurrentResult())
		{
			ValidOverlappingActors.Add(OtherActor);
			OnBeginOverlap.Broadcast(OtherActor);
			// DrawDebugSphere(GetWorld(), OtherActor->GetActorLocation(), 100.0f, 8, FColor::Green, false, 1.0f);
		}
	}
}

void UFPGAOverlapManager::HandleEndOverlap(AActor* OtherActor)
{
	if (AllOverlappingActors.Contains(OtherActor))
	{
		if (ValidOverlappingActors.Contains(OtherActor))
		{
			ValidOverlappingActors.Remove(OtherActor);
			OnEndOverlap.Broadcast(OtherActor);
			// DrawDebugSphere(GetWorld(), OtherActor->GetActorLocation(), 100.0f, 8, FColor::Red, false, 1.0f);
		}

		AllOverlappingActors.Remove(OtherActor);
	}
}

void UFPGAOverlapManager::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	HandleBeginOverlap(OtherActor);
}

void UFPGAOverlapManager::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	HandleEndOverlap(OtherActor);
}

void UFPGAOverlapManager::OnPrimitiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleBeginOverlap(OtherActor);
}

void UFPGAOverlapManager::OnPrimitiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HandleEndOverlap(OtherActor);
}

void UFPGAOverlapManager::OnFilterResultChanged(AActor* Target, bool bResult)
{
	if (bResult)
	{
		ValidOverlappingActors.Add(Target);
		OnBeginOverlap.Broadcast(Target);
	}
	else
	{
		ValidOverlappingActors.Remove(Target);
		OnEndOverlap.Broadcast(Target);
	}
}
