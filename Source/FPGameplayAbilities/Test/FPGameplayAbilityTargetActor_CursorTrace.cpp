// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPGameplayAbilityTargetActor_CursorTrace.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/LightWeightInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbilityTargetActor_CursorTrace)

// --------------------------------------------------------------------------------------------------------------------------------------------------------
//
//	AFPGameplayAbilityTargetActor_CursorTrace
//
// --------------------------------------------------------------------------------------------------------------------------------------------------------

AFPGameplayAbilityTargetActor_CursorTrace::AFPGameplayAbilityTargetActor_CursorTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FHitResult AFPGameplayAbilityTargetActor_CursorTrace::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AFPGameplayAbilityTargetActor_CursorTrace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	auto PC = GetWorld()->GetFirstPlayerController();
	auto LocalPlayer = PC->GetLocalPlayer();

	// Get mouse position
	FVector2D MousePosition;
	if (!LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
	{
		return FHitResult();
	}

	// Get ray
	FVector TraceStart;
	FVector WorldDirection;
	if (!UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, TraceStart, WorldDirection))
	{
		return FHitResult();
	}

	FVector TraceEnd = TraceStart + WorldDirection * 10000.0f;

	// FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	// FVector TraceEnd;
	// AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);		//Effective on server and launching client only

	// ------------------------------------------------------

	// FHitResult ReturnHitResult;
	// LineTraceWithFilter(ReturnHitResult, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);
	// //Default to end of trace line if we don't hit anything.
	// if (!ReturnHitResult.bBlockingHit)
	// {
	// 	ReturnHitResult.Location = TraceEnd;
	// }
	// if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	// {
	// 	const bool bHitActor = (ReturnHitResult.bBlockingHit && (ReturnHitResult.HitObjectHandle.IsValid()));
	// 	const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor) ? FLightWeightInstanceSubsystem::Get().GetLocation(ReturnHitResult.HitObjectHandle) : ReturnHitResult.Location;
	//
	// 	LocalReticleActor->SetActorLocation(ReticleLocation);
	// 	LocalReticleActor->SetIsTargetAnActor(bHitActor);
	// }

	TArray<FHitResult> HitResults;
	if (GetHitResultsUnderCursor(HitResults))
	{
		FHitResult FirstHitResult = HitResults[0];
#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green);
			DrawDebugSphere(GetWorld(), FirstHitResult.Location, 100.0f, 16, FColor::Green);
		}
#endif // ENABLE_DRAW_DEBUG

		return HitResults[0];
	}

	return FHitResult();
}

bool AFPGameplayAbilityTargetActor_CursorTrace::GetHitResultsUnderCursor(TArray<FHitResult>& OutHitResults)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return false;
	}

	// Get local player viewport
	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer || !LocalPlayer->ViewportClient)
	{
		return false;
	}

	// Get mouse position
	FVector2D MousePosition;
	if (!LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
	{
		return false;
	}

	// Get ray
	FVector WorldOrigin;
	FVector WorldDirection;
	if (!UGameplayStatics::DeprojectScreenToWorld(PC, MousePosition, WorldOrigin, WorldDirection))
	{
		return false;
	}

	World->LineTraceMultiByChannel(
		OutHitResults,
		WorldOrigin,
		WorldOrigin + WorldDirection * HitResultTraceDistance,
		DefaultClickTraceChannel
	);

	// finally check for the ground
	FHitResult GroundHitResult;
	if (PC->GetHitResultUnderCursorByChannel(GroundTraceChannel, false, GroundHitResult))
	{
		OutHitResults.Sort([&GroundHitResult](const FHitResult& A, const FHitResult& B)
		{
			return FVector::DistSquaredXY(A.Location, GroundHitResult.Location) < FVector::DistSquaredXY(B.Location, GroundHitResult.Location);
		});

		OutHitResults.Add(GroundHitResult);
	}

	return true;
}

void AFPGameplayAbilityTargetActor_CursorTrace::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPGameplayAbilityTargetActor_CursorTrace, HitResultTraceDistance);
	DOREPLIFETIME(AFPGameplayAbilityTargetActor_CursorTrace, DefaultClickTraceChannel);
	DOREPLIFETIME(AFPGameplayAbilityTargetActor_CursorTrace, GroundTraceChannel);
}

