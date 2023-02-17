// Copyright Epic Games, Inc. All Rights Reserved.
#include "FPTargetingSelectionTask_CursorTrace.h"

#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameplayTargetingSystem/TargetingSystem/TargetingSubsystem.h"
#include "GameplayTargetingSystem/Types/TargetingSystemLogs.h"
#include "Kismet/GameplayStatics.h"

#if ENABLE_DRAW_DEBUG
#include "Engine/Canvas.h"
#endif // ENABLE_DRAW_DEBUG


UFPTargetingSelectionTask_CursorTrace::UFPTargetingSelectionTask_CursorTrace(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bComplexTrace = false;
	bIgnoreSourceActor = false;
	bIgnoreInstigatorActor = false;
}

void UFPTargetingSelectionTask_CursorTrace::Init(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Init(TargetingHandle);
}

void UFPTargetingSelectionTask_CursorTrace::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Execute(TargetingHandle);

	//ETargetingTaskAsyncState::Initialized
	//SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Initialized);

	// if (IsAsyncTargetingRequest(TargetingHandle))
	// {
	// 	ExecuteAsyncTrace(TargetingHandle);
	// }
	// else
	// {
	// 	ExecuteImmediateTrace(TargetingHandle);
	// }

	UE_LOG(LogTemp, Warning, TEXT("Executing..."));
}

void UFPTargetingSelectionTask_CursorTrace::CancelAsync() const
{
	Super::CancelAsync();
}

FVector UFPTargetingSelectionTask_CursorTrace::GetSourceLocation_Implementation(const FTargetingRequestHandle& TargetingHandle) const
{
	if (const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (SourceContext->SourceActor)
		{
			if (APlayerController* PlayerController = SourceContext->SourceActor->GetWorld()->GetFirstPlayerController())
			{
				ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

				// Get mouse position
				FVector2D MousePosition;
				if (LocalPlayer && LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
				{
					FVector TraceStart;
					FVector WorldDirection;
					if (!UGameplayStatics::DeprojectScreenToWorld(PlayerController, MousePosition, TraceStart, WorldDirection))
					{
						return TraceStart;
					}
				}
			}
		}
	}


	return FVector::ZeroVector;
}

FVector UFPTargetingSelectionTask_CursorTrace::GetTraceDirection_Implementation(const FTargetingRequestHandle& TargetingHandle) const
{
	if (const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (SourceContext->SourceActor)
		{
			if (APlayerController* PlayerController = SourceContext->SourceActor->GetWorld()->GetFirstPlayerController())
			{
				ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

				// Get mouse position
				FVector2D MousePosition;
				if (LocalPlayer && LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
				{
					FVector TraceStart;
					FVector WorldDirection;
					if (!UGameplayStatics::DeprojectScreenToWorld(PlayerController, MousePosition, TraceStart, WorldDirection))
					{
						return WorldDirection;
					}
				}
			}
		}
	}

	return FVector::ZeroVector;
}

float UFPTargetingSelectionTask_CursorTrace::GetTraceLength_Implementation(const FTargetingRequestHandle& TargetingHandle) const
{
	return DefaultTraceLength.GetValue();
}

float UFPTargetingSelectionTask_CursorTrace::GetSweptTraceRadius_Implementation(const FTargetingRequestHandle& TargetingHandle) const
{
	return DefaultSweptTraceRadius.GetValue();
}

void UFPTargetingSelectionTask_CursorTrace::GetAdditionalActorsToIgnore_Implementation(const FTargetingRequestHandle& TargetingHandle, TArray<AActor*>& OutAdditionalActorsToIgnore) const
{
}

void UFPTargetingSelectionTask_CursorTrace::ExecuteImmediateTrace(const FTargetingRequestHandle& TargetingHandle) const
{
	if (UWorld* World = GetSourceContextWorld(TargetingHandle))
	{
#if ENABLE_DRAW_DEBUG
		ResetTraceResultsDebugString(TargetingHandle);
#endif // ENABLE_DRAW_DEBUG

		const FVector Start = GetSourceLocation(TargetingHandle);
		const FVector End = Start + (GetTraceDirection(TargetingHandle) * GetTraceLength(TargetingHandle));

		FCollisionQueryParams Params(SCENE_QUERY_STAT(ExecuteImmediateTrace), bComplexTrace);
		InitCollisionParams(TargetingHandle, Params);

		TArray<FHitResult> Hits;
		if (CollisionProfileName.Name != TEXT("NoCollision"))
		{
			if (TraceType == EFPTargetingTraceType::Sweep)
			{
				World->SweepMultiByProfile(Hits, Start, End, FQuat::Identity, CollisionProfileName.Name, FCollisionShape::MakeSphere(GetSweptTraceRadius(TargetingHandle)), Params);
			}
			else
			{
				World->LineTraceMultiByProfile(Hits, Start, End, CollisionProfileName.Name, Params);
			}
		}
		else
		{
			ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
			if (TraceType == EFPTargetingTraceType::Sweep)
			{
				World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(GetSweptTraceRadius(TargetingHandle)), Params);
			}
			else
			{
				World->LineTraceMultiByChannel(Hits, Start, End, CollisionChannel, Params);
			}
		}

#if ENABLE_DRAW_DEBUG
		if (UTargetingSubsystem::IsTargetingDebugEnabled())
		{
			DrawDebugLine(World, Start, End, FColor::Green, false, 30.0f, 0, 2.0f);
		}
#endif // ENABLE_DRAW_DEBUG

		ProcessHitResults(TargetingHandle, Hits);
	}

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}

void UFPTargetingSelectionTask_CursorTrace::ExecuteAsyncTrace(const FTargetingRequestHandle& TargetingHandle) const
{
	if (UWorld* World = GetSourceContextWorld(TargetingHandle))
	{
		// AActor* SourceActor = nullptr;
		// if (const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
		// {
		// 	SourceActor = SourceContext->SourceActor;
		// }
		const FVector Start = GetSourceLocation(TargetingHandle);
		const FVector End = Start + (GetTraceDirection(TargetingHandle) * GetTraceLength(TargetingHandle));

		FCollisionQueryParams Params(SCENE_QUERY_STAT(ExecuteAsyncTrace), bComplexTrace);
		InitCollisionParams(TargetingHandle, Params);

		FTraceDelegate Delegate = FTraceDelegate::CreateUObject(this, &UFPTargetingSelectionTask_CursorTrace::HandleAsyncTraceComplete, TargetingHandle);
		if (CollisionProfileName.Name != TEXT("NoCollision"))
		{
			if (TraceType == EFPTargetingTraceType::Sweep)
			{
				World->AsyncSweepByProfile(EAsyncTraceType::Multi, Start, End, FQuat::Identity, CollisionProfileName.Name, FCollisionShape::MakeSphere(GetSweptTraceRadius(TargetingHandle)), Params, &Delegate);
			}
			else
			{
				World->AsyncLineTraceByProfile(EAsyncTraceType::Multi, Start, End, CollisionProfileName.Name, Params, &Delegate);
			}
		}
		else
		{
			ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
			if (TraceType == EFPTargetingTraceType::Sweep)
			{
				World->AsyncSweepByChannel(EAsyncTraceType::Multi, Start, End, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(GetSweptTraceRadius(TargetingHandle)), Params, FCollisionResponseParams::DefaultResponseParam, &Delegate);
			}
			else
			{
				World->AsyncLineTraceByChannel(EAsyncTraceType::Multi, Start, End, CollisionChannel, Params, FCollisionResponseParams::DefaultResponseParam, &Delegate);
			}
		}
	}
	else
	{
		SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
	}
}

void UFPTargetingSelectionTask_CursorTrace::HandleAsyncTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum, FTargetingRequestHandle TargetingHandle) const
{
	if (TargetingHandle.IsValid())
	{
#if ENABLE_DRAW_DEBUG
		ResetTraceResultsDebugString(TargetingHandle);

		if (UTargetingSubsystem::IsTargetingDebugEnabled())
		{
			if (UWorld* World = GetSourceContextWorld(TargetingHandle))
			{
				DrawDebugLine(World, InTraceDatum.Start, InTraceDatum.End, FColor::Green, false, 30.0f, 0, 2.0f);
			}
		}
#endif // ENABLE_DRAW_DEBUG

		ProcessHitResults(TargetingHandle, InTraceDatum.OutHits);
	}

	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}

void UFPTargetingSelectionTask_CursorTrace::ProcessHitResults(const FTargetingRequestHandle& TargetingHandle, const TArray<FHitResult>& Hits) const
{
	if (TargetingHandle.IsValid() && Hits.Num() > 0)
	{
		FTargetingDefaultResultsSet& TargetingResults = FTargetingDefaultResultsSet::FindOrAdd(TargetingHandle);
		for (const FHitResult& HitResult : Hits)
		{
			if (!HitResult.GetActor())
			{
				continue;
			}

			bool bAddResult = true;
			for (const FTargetingDefaultResultData& ResultData : TargetingResults.TargetResults)
			{
				if (ResultData.HitResult.GetActor() == HitResult.GetActor())
				{
					bAddResult = false;
					break;
				}
			}

			if (bAddResult)
			{
				FTargetingDefaultResultData* ResultData = new(TargetingResults.TargetResults) FTargetingDefaultResultData();
				ResultData->HitResult = HitResult;
			}
		}

#if ENABLE_DRAW_DEBUG
		BuildTraceResultsDebugString(TargetingHandle, TargetingResults.TargetResults);
#endif // ENABLE_DRAW_DEBUG
	}
}

void UFPTargetingSelectionTask_CursorTrace::InitCollisionParams(const FTargetingRequestHandle& TargetingHandle, FCollisionQueryParams& OutParams) const
{
	if (const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (bIgnoreSourceActor && SourceContext->SourceActor)
		{
			OutParams.AddIgnoredActor(SourceContext->SourceActor);
		}

		if (bIgnoreInstigatorActor && SourceContext->InstigatorActor)
		{
			OutParams.AddIgnoredActor(SourceContext->InstigatorActor);
		}

		TArray<AActor*> AdditionalActorsToIgnoreArray;
		GetAdditionalActorsToIgnore(TargetingHandle, AdditionalActorsToIgnoreArray);

		if (AdditionalActorsToIgnoreArray.Num() > 0)
		{
			OutParams.AddIgnoredActors(AdditionalActorsToIgnoreArray);
		}
	}
}

#if WITH_EDITOR
bool UFPTargetingSelectionTask_CursorTrace::CanEditChange(const FProperty* InProperty) const
{
	bool bCanEdit = Super::CanEditChange(InProperty);

	if (bCanEdit && InProperty)
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UFPTargetingSelectionTask_CursorTrace, DefaultSweptTraceRadius.GetValue()))
		{
			return (TraceType == EFPTargetingTraceType::Sweep);
		}

		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UFPTargetingSelectionTask_CursorTrace, TraceChannel))
		{
			return (CollisionProfileName.Name == TEXT("NoCollision"));
		}
	}

	return true;
}
#endif // WITH_EDITOR

#if ENABLE_DRAW_DEBUG

void UFPTargetingSelectionTask_CursorTrace::DrawDebug(UTargetingSubsystem* TargetingSubsystem, FTargetingDebugInfo& Info, const FTargetingRequestHandle& TargetingHandle, float XOffset, float YOffset, int32 MinTextRowsToAdvance) const
{
#if WITH_EDITORONLY_DATA
	if (UTargetingSubsystem::IsTargetingDebugEnabled())
	{
		FTargetingDebugData& DebugData = FTargetingDebugData::FindOrAdd(TargetingHandle);
		FString& ScratchPadString = DebugData.DebugScratchPadStrings.FindOrAdd(GetNameSafe(this));
		if (!ScratchPadString.IsEmpty())
		{
			if (Info.Canvas)
			{
				Info.Canvas->SetDrawColor(FColor::Yellow);
			}

			FString TaskString = FString::Printf(TEXT("Results : %s"), *ScratchPadString);
			TargetingSubsystem->DebugLine(Info, TaskString, XOffset, YOffset, MinTextRowsToAdvance);


			FTargetingDefaultResultsSet& TargetingResults = FTargetingDefaultResultsSet::FindOrAdd(TargetingHandle);
			for (FTargetingDefaultResultData& TargetResult : TargetingResults.TargetResults)
			{
				FVector_NetQuantize HitLocation = TargetResult.HitResult.Location;
				DrawDebugSphere(GetWorld(), HitLocation, 10, 8, FColor::Green);
			}

			TargetingSubsystem->DebugLine(Info, TaskString, XOffset, YOffset, MinTextRowsToAdvance);
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void UFPTargetingSelectionTask_CursorTrace::BuildTraceResultsDebugString(const FTargetingRequestHandle& TargetingHandle, const TArray<FTargetingDefaultResultData>& TargetResults) const
{
#if WITH_EDITORONLY_DATA
	if (UTargetingSubsystem::IsTargetingDebugEnabled())
	{
		FTargetingDebugData& DebugData = FTargetingDebugData::FindOrAdd(TargetingHandle);
		FString& ScratchPadString = DebugData.DebugScratchPadStrings.FindOrAdd(GetNameSafe(this));

		for (const FTargetingDefaultResultData& TargetData : TargetResults)
		{
			if (const AActor* Target = TargetData.HitResult.GetActor())
			{
				if (ScratchPadString.IsEmpty())
				{
					ScratchPadString = FString::Printf(TEXT("%s"), *GetNameSafe(Target));
				}
				else
				{
					ScratchPadString += FString::Printf(TEXT(", %s"), *GetNameSafe(Target));
				}
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

void UFPTargetingSelectionTask_CursorTrace::ResetTraceResultsDebugString(const FTargetingRequestHandle& TargetingHandle) const
{
#if WITH_EDITORONLY_DATA
	FTargetingDebugData& DebugData = FTargetingDebugData::FindOrAdd(TargetingHandle);
	FString& ScratchPadString = DebugData.DebugScratchPadStrings.FindOrAdd(GetNameSafe(this));
	ScratchPadString.Reset();
#endif // WITH_EDITORONLY_DATA
}

#endif // ENABLE_DRAW_DEBUG


