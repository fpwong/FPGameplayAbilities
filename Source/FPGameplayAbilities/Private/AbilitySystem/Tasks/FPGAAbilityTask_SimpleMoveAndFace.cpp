// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Tasks/FPGAAbilityTask_SimpleMoveAndFace.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"

UFPGAAbilityTask_SimpleMoveAndFace::UFPGAAbilityTask_SimpleMoveAndFace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bSimulatedTask = false;
	bIsPausable = true;
	bHasFired = false;
}

UFPGAAbilityTask_SimpleMoveAndFace* UFPGAAbilityTask_SimpleMoveAndFace::SimpleMoveAndFace(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	float MaxRange,
	FVector TargetLocation,
	AActor* TargetActor,
	bool bEndOnReaching)
{
	UFPGAAbilityTask_SimpleMoveAndFace* MyObj = NewAbilityTask<UFPGAAbilityTask_SimpleMoveAndFace>(OwningAbility, TaskInstanceName);

	MyObj->TargetActor = TargetActor;
	MyObj->TargetLocation = TargetLocation;
	MyObj->Range = MaxRange;
	MyObj->SetOwnerCharacter();
	MyObj->bEndOnReachingTarget = bEndOnReaching;

	return MyObj;
}

void UFPGAAbilityTask_SimpleMoveAndFace::SetOwnerCharacter()
{
	OwnerCharacter = Cast<ACharacter>(GetOwnerActor());
}

void UFPGAAbilityTask_SimpleMoveAndFace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	FVector Location = TargetActor != nullptr ? TargetActor->GetActorLocation() : TargetLocation;
	Location.Z = OwnerCharacter->GetActorLocation().Z;

	const FVector DeltaPos = Location - OwnerCharacter->GetActorLocation();

	float Distance = DeltaPos.Size2D();

	if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
	{
		Distance -= TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}

	Distance -= OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();

	// check range
	const bool bInRange = Distance <= Range;

	// check rotation
	const FRotator DesiredRotation = (Location - OwnerCharacter->GetActorLocation()).Rotation();
	const int AngleDelta = FMath::Abs(DesiredRotation.Yaw - OwnerCharacter->GetActorRotation().Yaw);

	const bool bTargetingSelf = TargetActor == GetOwnerActor();
	const bool bIsFacing = bTargetingSelf || AngleDelta <= 25;

	if (OwnerCharacter->IsLocallyControlled())
	{
		if (!bTargetingSelf)
		{
			OwnerCharacter->GetController()->SetControlRotation(DeltaPos.Rotation());
		}

		if (!bInRange)
		{
			OwnerCharacter->AddMovementInput(DeltaPos);
		}

		DrawDebugSphere(GetWorld(), TargetLocation, 5, 4, FColor::Green, false, -1.f, 0, 2.f);
	}

	if (bInRange && bIsFacing)
	{
		if (bEndOnReachingTarget)
		{
			EndTask();
		}

		//if (!bHasFired)
		{
			//bHasFired = true;
			OnFinish.Broadcast();
		}
	}
}

void UFPGAAbilityTask_SimpleMoveAndFace::Activate()
{
	Super::Activate();
	SetWaitingOnAvatar();
}

void UFPGAAbilityTask_SimpleMoveAndFace::OnDestroy(bool AbilityEnding)
{
	Super::OnDestroy(AbilityEnding);
}

void UFPGAAbilityTask_SimpleMoveAndFace::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);

	SetOwnerCharacter();
}
