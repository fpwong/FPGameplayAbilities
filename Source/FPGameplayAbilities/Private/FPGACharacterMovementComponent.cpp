// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGACharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UFPGACharacterMovementComponent::UFPGACharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MovementMode = EMovementMode::MOVE_NavWalking;
}

// Called when the game starts
void UFPGACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

void UFPGACharacterMovementComponent::ServerMoveHandleClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	Super::ServerMoveHandleClientError(ClientTimeStamp, DeltaTime, Accel, RelativeClientLocation, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

bool UFPGACharacterMovementComponent::ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	// if (AbilitySystem)
	// {
	// 	if (AbilitySystem->HasAnyMatchingGameplayTags(BlockedMovementTags))
	// 	{
	// 		return true;
	// 	}
	// }

	return Super::ServerCheckClientError(ClientTimeStamp, DeltaTime, Accel, ClientWorldLocation, RelativeClientLocation, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

void UFPGACharacterMovementComponent::PerformMovement(float DeltaTime)
{
	if (AbilitySystem)
	{
		if (AbilitySystem->HasAnyMatchingGameplayTags(BlockedMovementTags))
		{
			ClearAccumulatedForces();
			// StopActiveMovement();
			return;
		}
	}

	Super::PerformMovement(DeltaTime);
}

void UFPGACharacterMovementComponent::ClientAdjustPosition_Implementation(
	float TimeStamp,
	FVector NewLocation,
	FVector NewVelocity,
	UPrimitiveComponent* NewBase, 
	FName NewBaseBoneName,
	bool bHasBase,
	bool bBaseRelativePosition,
	uint8 ServerMovementMode,
	TOptional<FRotator> OptionalRotation)
{
	// this here does the corrections!
	Super::ClientAdjustPosition_Implementation(TimeStamp, NewLocation, NewVelocity, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode, OptionalRotation);
	return;

	// if (!HasValidData() || !IsActive())
	// {
	// 	return;
	// }
	//
	//
	// FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	// check(ClientData);
	//
	// // Make sure the base actor exists on this client.
	// const bool bUnresolvedBase = bHasBase && (NewBase == NULL);
	// if (bUnresolvedBase)
	// {
	// 	if (bBaseRelativePosition)
	// 	{
	// 		UE_LOG(LogNetPlayerMovement, Warning, TEXT("ClientAdjustPosition_Implementation could not resolve the new relative movement base actor, ignoring server correction! Client currently at world location %s on base %s"),
	// 			*UpdatedComponent->GetComponentLocation().ToString(), *GetNameSafe(GetMovementBase()));
	// 		return;
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogNetPlayerMovement, Verbose, TEXT("ClientAdjustPosition_Implementation could not resolve the new absolute movement base actor, but WILL use the position!"));
	// 	}
	// }
	//
	// // Ack move if it has not expired.
	// int32 MoveIndex = ClientData->GetSavedMoveIndex(TimeStamp);
	// if( MoveIndex == INDEX_NONE )
	// {
	// 	if( ClientData->LastAckedMove.IsValid() )
	// 	{
	// 		UE_LOG(LogNetPlayerMovement, Log,  TEXT("ClientAdjustPosition_Implementation could not find Move for TimeStamp: %f, LastAckedTimeStamp: %f, CurrentTimeStamp: %f"), TimeStamp, ClientData->LastAckedMove->TimeStamp, ClientData->CurrentTimeStamp);
	// 	}
	// 	return;
	// }
	//
	// ClientData->AckMove(MoveIndex, *this);
	//
	// FVector WorldShiftedNewLocation;
	// //  Received Location is relative to dynamic base
	// if (bBaseRelativePosition)
	// {
	// 	FVector BaseLocation;
	// 	FQuat BaseRotation;
	// 	MovementBaseUtility::GetMovementBaseTransform(NewBase, NewBaseBoneName, BaseLocation, BaseRotation); // TODO: error handling if returns false		
	// 	WorldShiftedNewLocation = NewLocation + BaseLocation;
	// }
	// else
	// {
	// 	WorldShiftedNewLocation = FRepMovement::RebaseOntoLocalOrigin(NewLocation, this);
	// }
	//
	//
	// // Trigger event
	// // OnClientCorrectionReceived(*ClientData, TimeStamp, WorldShiftedNewLocation, NewVelocity, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);
	//
	// // Trust the server's positioning.
	// if (UpdatedComponent)
	// {
	// 	// ~~ THIS WAS THE ONLY CHANGE
	// 	// UpdatedComponent->SetWorldLocation(WorldShiftedNewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	// 	// ~~ THIS WAS THE ONLY CHANGE
	//
	//
	// 	// ~~ MY CHANGE
	// 	FVector Delta = UpdatedComponent->GetComponentLocation() - WorldShiftedNewLocation;
	// 	FVector InterpLocation;
	// 	if (Delta.Size() >= 25)
	// 	{
	// 		InterpLocation = FMath::VInterpTo(UpdatedComponent->GetComponentLocation(), WorldShiftedNewLocation, GetWorld()->DeltaTimeSeconds, 12.0f);
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Set the location correctly!"));
	// 		InterpLocation = WorldShiftedNewLocation;
	// 	}
	// 	UpdatedComponent->SetWorldLocation(InterpLocation, false, nullptr, ETeleportType::TeleportPhysics);
	// 	// ~~ MY CHANGE
	// }
	//
	// Velocity = NewVelocity;
	//
	// // Trust the server's movement mode
	// UPrimitiveComponent* PreviousBase = CharacterOwner->GetMovementBase();
	// ApplyNetworkMovementMode(ServerMovementMode);
	//
	// // Set base component
	// UPrimitiveComponent* FinalBase = NewBase;
	// FName FinalBaseBoneName = NewBaseBoneName;
	// if (bUnresolvedBase)
	// {
	// 	check(NewBase == NULL);
	// 	check(!bBaseRelativePosition);
	// 	
	// 	// We had an unresolved base from the server
	// 	// If walking, we'd like to continue walking if possible, to avoid falling for a frame, so try to find a base where we moved to.
	// 	if (PreviousBase && UpdatedComponent)
	// 	{
	// 		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
	// 		if (CurrentFloor.IsWalkableFloor())
	// 		{
	// 			FinalBase = CurrentFloor.HitResult.Component.Get();
	// 			FinalBaseBoneName = CurrentFloor.HitResult.BoneName;
	// 		}
	// 		else
	// 		{
	// 			FinalBase = nullptr;
	// 			FinalBaseBoneName = NAME_None;
	// 		}
	// 	}
	// }
	// SetBase(FinalBase, FinalBaseBoneName);
	//
	// // Update floor at new location
	// UpdateFloorFromAdjustment();
	// bJustTeleported = true;
	//
	// // Even if base has not changed, we need to recompute the relative offsets (since we've moved).
	// SaveBaseLocation();
	//
	// LastUpdateLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	// LastUpdateRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
	// LastUpdateVelocity = Velocity;
	//
	// UpdateComponentVelocity();
	// ClientData->bUpdatePosition = true;
}

void UFPGACharacterMovementComponent::RequestPathMove(const FVector& MoveInput)
{
	AddInputVector(MoveInput, false);
}

void UFPGACharacterMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	AddInputVector(MoveVelocity, false);
}

void UFPGACharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// // if (GetOwnerRole() == ENetRole::ROLE_Authority)
	// {
	// 	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
	// 	if (ClientData)
	// 	{
	// 		for (TSharedPtr<FSavedMove_Character> SavedMove_Character : ClientData->SavedMoves)
	// 		{
	// 			DrawDebugSphere(GetWorld(), SavedMove_Character->StartLocation, 12, 4, FColor::Red);
	// 		}
	// 	}
	// }

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UFPGACharacterMovementComponent::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
	{
		// SCOPE_CYCLE_COUNTER(STAT_CharUpdateAcceleration);

		// We need to check the jump state before adjusting input acceleration, to minimize latency
		// and to make sure acceleration respects our potentially new falling state.
		CharacterOwner->CheckJumpInput(DeltaSeconds);

		// apply input to acceleration
		Acceleration = ScaleInputAcceleration(ConstrainInputAcceleration(InputVector));
		AnalogInputModifier = ComputeAnalogInputModifier();
	}

	if (CharacterOwner->GetLocalRole() == ROLE_Authority)
	{
		PerformMovement(DeltaSeconds);
	}
	else if (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy && IsNetMode(NM_Client))
	{
		ReplicateMoveToServer(DeltaSeconds, Acceleration);
	}
}

float UFPGACharacterMovementComponent::GetMaxSpeed() const
{
	// TODO: register to on change callback
	if (AbilitySystem)
	{
		bool bFound = false;
		float MovementSpeedValue = AbilitySystem->GetGameplayAttributeValue(MovementSpeedAttribute, bFound);
		if (bFound)
		{
			return MovementSpeedValue;
		}
	}
	return Super::GetMaxSpeed();
}

void UFPGACharacterMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	if (AbilitySystem)
	{
		if (AbilitySystem->HasAnyMatchingGameplayTags(BlockedMovementTags))
		{
			return;
		}
	}

	Super::AddInputVector(WorldVector, bForce);
}
