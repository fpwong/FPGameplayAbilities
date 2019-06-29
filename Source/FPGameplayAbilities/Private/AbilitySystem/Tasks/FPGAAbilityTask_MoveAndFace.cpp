// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Tasks/FPGAAbilityTask_MoveAndFace.h"
#include "FPGACharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

UFPGAAbilityTask_MoveAndFace::UFPGAAbilityTask_MoveAndFace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bSimulatedTask = true;
	bIsPausable = true;
	bReachedGoal = false;
	PFollowComp = nullptr;
}

UFPGAAbilityTask_MoveAndFace* UFPGAAbilityTask_MoveAndFace::MoveAndFace(UGameplayAbility* OwningAbility, FName TaskInstanceName, float MaxRange, FVector TargetLocation, AActor* TargetActor)
{
	UFPGAAbilityTask_MoveAndFace* MyObj = NewAbilityTask<UFPGAAbilityTask_MoveAndFace>(OwningAbility, TaskInstanceName);

	MyObj->TargetActor = TargetActor;
	MyObj->TargetLocation = TargetLocation;
	MyObj->Range = MaxRange;
	MyObj->SetOwnerCharacter();

	return MyObj;
}

void UFPGAAbilityTask_MoveAndFace::SetOwnerCharacter()
{
	OwnerCharacter = Cast<ACharacter>(GetOwnerActor());
}

void UFPGAAbilityTask_MoveAndFace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (OwnerCharacter == nullptr)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("OwnerCharacter is null?"), true, false);
		//EndTask();
		return;
	}

	FVector Location = NavLocation; //TargetActor != nullptr ? TargetActor->GetActorLocation() : TargetLocation;
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
	bool bIsFacing = false;

	const bool bTargetingSelf = TargetActor == GetOwnerActor();

	if (TargetActor == nullptr)
	{
		bIsFacing = true;

		if (OwnerCharacter->IsLocallyControlled())
		{
			if (OwnerCharacter->GetVelocity().Size2D() > 0)
			{
				OwnerCharacter->GetController()->SetControlRotation(OwnerCharacter->GetVelocity().Rotation());
			}
		}
	}
	else
	{
		FVector TargetActorLocation = TargetActor->GetActorLocation();
		TargetActorLocation.Z = OwnerCharacter->GetActorLocation().Z;
		const FRotator DesiredRotation = (TargetActorLocation - OwnerCharacter->GetActorLocation()).Rotation();
		const int AngleDelta = FMath::Abs(DesiredRotation.Yaw - OwnerCharacter->GetActorRotation().Yaw);

		bIsFacing = bTargetingSelf || AngleDelta <= 25;

		if (OwnerCharacter->IsLocallyControlled())
		{
			OwnerCharacter->GetController()->SetControlRotation(DesiredRotation);
		}
	}

	//if (OwnerCharacter->IsLocallyControlled())
	//{
	//	if (!bTargetingSelf)
	//	{
	//		if (!bInRange) // rotate to velocity
	//		{
	//			OwnerCharacter->GetController()->SetControlRotation(OwnerCharacter->GetVelocity().Rotation());
	//		}
	//		else if (TargetActor != nullptr) // rotate to target
	//		{
	//			OwnerCharacter->GetController()->SetControlRotation(DeltaPos.Rotation());
	//		}
	//	}

	//	//if (!bInRange)
	//	//{
	//	//	OwnerCharacter->AddMovementInput(DeltaPos);
	//	//}

	//	//DrawDebugSphere(GetWorld(), TargetLocation, 5, 4, FColor::Green, false, -1.f, 0, 2.f);
	//}

	if (bInRange && bIsFacing)
		//if (bReachedGoal && bIsFacing)
	{
		PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		EndTask();
		OnFinish.Broadcast();
	}

	if (PFollowComp != nullptr)
	{
		FNavPathSharedPtr Path = PFollowComp->GetPath();
		if (Path.IsValid())
		{
			auto Points = Path->GetPathPoints();

			UE_LOG(LogTemp, Warning, TEXT("PFOLLOW points %d"), Points.Num());

			for (FVector Point : Path->GetPathPoints())
			{
				DrawDebugSphere(GetWorld(), Point, 10, 8, FColor::Red, false, 5.f, 0, 2.f);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Path is not valid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PFollow is nullptr!!"));
	}
}

void UFPGAAbilityTask_MoveAndFace::Activate()
{
	Super::Activate();

	if (TargetActor != nullptr)
	{
		MoveRequest = FAIMoveRequest(TargetActor);
	}
	else
	{
		MoveRequest = FAIMoveRequest(GetDesiredLocation());
	}

	AController* Controller = OwnerCharacter->GetController();

	NavSys = Controller ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(Controller->GetWorld()) : nullptr;

	PFollowComp = Controller->FindComponentByClass<UPathFollowingComponent>();
	if (PFollowComp == nullptr)
	{
		PFollowComp = NewObject<UPathFollowingComponent>(Controller);
		PFollowComp->RegisterComponentWithWorld(Controller->GetWorld());
		PFollowComp->Initialize();
	}

	const ANavigationData* NavData = NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef());
	FPathFindingQuery Query(Controller, *NavData, Controller->GetNavAgentLocation(), TargetLocation);
	FPathFindingResult Result = NavSys->FindPathSync(Query);
	if (Result.IsSuccessful())
	{
		//if (TargetActor != nullptr)

		PFollowComp->RequestMove(FAIMoveRequest(TargetLocation), Result.Path);

		if (PFollowComp->HasValidPath())
		{
			FNavPathSharedPtr Path = PFollowComp->GetPath();
			if (Path.IsValid())
			{
				auto Points = Path->GetPathPoints();

				UE_LOG(LogTemp, Warning, TEXT("PFOLLOW points %d"), Points.Num());

				for (FVector Point : Path->GetPathPoints())
				{
					DrawDebugSphere(GetWorld(), Point, 10, 8, FColor::Red, false, 5.f, 0, 2.f);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Path invalid 2"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Path invalid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Result invalid"));
	}

	SetWaitingOnAvatar();

	//StartMoving();
}

void UFPGAAbilityTask_MoveAndFace::OnDestroy(bool AbilityEnding)
{
	Super::OnDestroy(AbilityEnding);
}

void UFPGAAbilityTask_MoveAndFace::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);

	SetOwnerCharacter();
}

void UFPGAAbilityTask_MoveAndFace::StartMoving()
{
	if (!OwnerCharacter->IsLocallyControlled())
		return;

	///*if (TargetActor)
	//{
	//	UAIBlueprintHelperLibrary::SimpleMoveToActor(OwnerCharacter->GetController(), TargetActor);
	//}*/
	////else
	////{
	////	UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerCharacter->GetController(), TargetLocation);
	////}

	//AController* Controller = OwnerCharacter->GetController();

	//UNavigationSystemV1* NavSys = Controller ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(Controller->GetWorld()) : nullptr;

	//if (NavSys == nullptr || Controller == nullptr)
	//	return;

	//UE_LOG(LogTemp, Warning, TEXT("Cliendside navigation? %d"), NavSys->ShouldAllowClientSideNavigation());

	//FNavLocation ProjectedLocation;
	//const FNavAgentProperties& AgentProps = Controller->GetNavAgentPropertiesRef();

	//FVector DesiredLocation = GetDesiredLocation();

	//DrawDebugSphere(GetWorld(), DesiredLocation, 5, 4, FColor::Green, false, 5.f, 0, 2.f);

	//if (!NavSys->ProjectPointToNavigation(GetDesiredLocation(), ProjectedLocation, INVALID_NAVEXTENT, &AgentProps))
	//{
	//	NavLocation = GetOwnerActor()->GetActorLocation();
	//	UE_LOG(LogTemp, Warning, TEXT("MoveAndFace: Failed to project to navmesh"));
	//	return;
	//}

	//NavLocation = ProjectedLocation.Location;

	//AAIController* AsAIController = Cast<AAIController>(Controller);
	////UPathFollowingComponent* PFollowComp = nullptr;
	//
	//PFollowComp = Controller->FindComponentByClass<UPathFollowingComponent>();
	//if (PFollowComp == nullptr)
	//{
	//	PFollowComp = NewObject<UPathFollowingComponent>(Controller);
	//	PFollowComp->RegisterComponentWithWorld(Controller->GetWorld());
	//	PFollowComp->Initialize();
	//}

	//const bool bAlreadyAtGoal = PFollowComp->HasReached(ProjectedLocation.Location, EPathFollowingReachMode::OverlapAgentAndGoal, Range);
	//
	//// script source, keep only one move request at time
	//if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
	//{
	//	PFollowComp->AbortMove(*NavSys, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest, FAIRequestID::AnyRequest, bAlreadyAtGoal ? EPathFollowingVelocityMode::Reset : EPathFollowingVelocityMode::Keep);
	//}

	//if (bAlreadyAtGoal)
	//{
	//	bReachedGoal = true;
	//	PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
	//}
	//else
	//{
	//	const ANavigationData* NavData = NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef());
	//	if (NavData)
	//	{
	//		FPathFindingQuery Query(Controller, *NavData, Controller->GetNavAgentLocation(), ProjectedLocation.Location);
	//		FPathFindingResult Result = NavSys->FindPathSync(Query);
	//		if (Result.IsSuccessful())
	//		{
	//			if (TargetActor != nullptr)
	//			{
	//				Result.Path->SetGoalActorObservation(*TargetActor, 100.0f);
	//				PFollowComp->RequestMove(FAIMoveRequest(TargetActor), Result.Path);
	//			}
	//			else
	//			{
	//				PFollowComp->RequestMove(FAIMoveRequest(ProjectedLocation.Location), Result.Path);
	//			}
	//		}
	//		else if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
	//		{
	//			PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
	//		}
	//	}
	//}
}

FVector UFPGAAbilityTask_MoveAndFace::GetDesiredLocation()
{
	return TargetActor != nullptr ? TargetActor->GetActorLocation() : TargetLocation;
}
