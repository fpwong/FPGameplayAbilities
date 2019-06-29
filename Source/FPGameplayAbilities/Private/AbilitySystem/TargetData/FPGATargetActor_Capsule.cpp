// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/TargetData/FPGATargetActor_Capsule.h"

#include "GameFramework/Pawn.h"
#include "WorldCollision.h"
#include "Abilities/GameplayAbility.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AFPGATargetActor_Capsule::AFPGATargetActor_Capsule(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Radius = 200;
	HalfHeight = 400;
	Angle = 45;

	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.TickGroup = TG_PrePhysics;
	//ShouldProduceTargetDataOnServer = true;
	//bDestroyOnConfirmation = false;
}

void AFPGATargetActor_Capsule::StartTargeting(UGameplayAbility* Ability)
{
	AngleCos = FMath::Cos(FMath::Clamp(FMath::DegreesToRadians(Angle), 0.f, PI));

	Super::StartTargeting(Ability);

	if (ReticleClass)
	{
		AGameplayAbilityWorldReticle* SpawnedReticleActor = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, GetActorLocation(), GetActorRotation());
		if (SpawnedReticleActor)
		{
			if (APlayerController* PC = Cast<APlayerController>(MasterPC))
			{
				SpawnedReticleActor->InitializeReticle(this, PC, ReticleParams);
				ReticleActor = SpawnedReticleActor;

				// This is to catch cases of playing on a listen server where we are using a replicated reticle actor.
				// (In a client controlled player, this would only run on the client and therefor never replicate. If it runs
				// on a listen server, the reticle actor may replicate. We want consistancy between client/listen server players.
				// Just saying 'make the reticle actor non replicated' isnt a good answer, since we want to mix and match reticle
				// actors and there may be other targeting types that want to replicate the same reticle actor class).
				if (!ShouldProduceTargetDataOnServer)
				{
					SpawnedReticleActor->SetReplicates(false);
				}
			}
		}
	}
}

void AFPGATargetActor_Capsule::ConfirmTargetingAndContinue()
{
	// Run on server and launching client only
	if (!OwningAbility)
		return;

	if (bDebug)
		DrawDebugFromHit(RunTrace(), 2.f);

	TArray<TWeakObjectPtr<AActor>> HitActors;
	for (AActor* CursorHit : RunTrace())
	{
		UE_LOG(LogTemp, Warning, TEXT("CursorHit %s"), *CursorHit->GetName());
		HitActors.Add(CursorHit);
	}

	TargetDataReadyDelegate.Broadcast(StartLocation.MakeTargetDataHandleFromActors(HitActors));
}

void AFPGATargetActor_Capsule::Tick(float DeltaSeconds)
{
	if (bDebug)
		DrawDebugFromHit(RunTrace(), -1.f);

	if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActor.Get())
	{
		LocalReticleActor->SetActorLocation(GetTraceLocation());
	}
}

TArray<AActor*> AFPGATargetActor_Capsule::RunTrace()
{
	APawn* Pawn = Cast<APawn>(SourceActor);

	check(Pawn);

	TArray<UPrimitiveComponent*> OverlapComponents;
	UClass* FilterClass = AActor::StaticClass();
	TArray<AActor*> IgnoredActors;
	TArray<AActor*> HitActors;

	FVector TraceLocation = GetTraceLocation();

	UKismetSystemLibrary::CapsuleOverlapActors(
		GetWorld(),
		TraceLocation,
		Radius,
		HalfHeight,
		ObjectTypes,
		FilterClass,
		IgnoredActors,
		HitActors);

	float AngleCosCopy = AngleCos;
	FVector SourceForward = SourceActor->GetActorForwardVector();
	FFPGAGameplayTargetDataFilter& FilterRef = Filter;
	const auto& HitActorsFilter = [&FilterRef, &SourceForward, &TraceLocation, &AngleCosCopy](AActor* Actor)
	{
		const FVector DirectionToTarget = (Actor->GetActorLocation() - TraceLocation).GetSafeNormal();
		const float AngleBetween = FVector::DotProduct(DirectionToTarget, SourceForward);
		return AngleBetween <= AngleCosCopy || !FilterRef.FilterPassesForActor(Actor);
	};

	HitActors.RemoveAll(HitActorsFilter);

	return HitActors;
}

FVector AFPGATargetActor_Capsule::GetTraceLocation()
{
	if (StartLocation.LocationType == EGameplayAbilityTargetingLocationType::LiteralTransform)
	{
		return StartLocation.LiteralTransform.GetLocation();
	}

	if (StartLocation.LocationType == EGameplayAbilityTargetingLocationType::ActorTransform && StartLocation.SourceActor != nullptr)
	{
		return StartLocation.SourceActor->GetActorLocation();
	}

	return FVector::ZeroVector;
}

void AFPGATargetActor_Capsule::DrawDebugFromHit(const TArray<AActor*>& HitActors, float Duration)
{
	const FVector Left = SourceActor->GetActorForwardVector().RotateAngleAxis(-Angle, FVector::UpVector) * Radius;
	const FVector Right = SourceActor->GetActorForwardVector().RotateAngleAxis(Angle, FVector::UpVector) * Radius;
	DrawDebugLine(GetWorld(), GetTraceLocation(), GetTraceLocation() + Left, FColor::Red, false, Duration, 0, 1);
	DrawDebugLine(GetWorld(), GetTraceLocation(), GetTraceLocation() + Right, FColor::Red, false, Duration, 0, 1);
	DrawDebugCircle(GetWorld(), GetTraceLocation(), Radius, 32, FColor::Red, false, Duration, 0, 1, FVector(0, 1, 0), FVector(1, 0, 0), false);
	DrawDebugCapsule(GetWorld(), GetTraceLocation(), HalfHeight, Radius, FQuat::MakeFromEuler(FVector::ZeroVector), FColor::White, false, Duration, 0, 1);

	for (AActor* const& Actor : HitActors)
	{
		DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), 24, 32, FColor(255, 0, 0));
	}
}

void AFPGATargetActor_Capsule::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ReticleActor.IsValid())
	{
		ReticleActor.Get()->Destroy();
	}
}

void AFPGATargetActor_Capsule::CancelTargeting()
{
	AFPGAGameplayAbilityTargetActor::CancelTargeting();
}
