// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGACharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

// Sets default values for this component's properties
UFPGACharacterMovementComponent::UFPGACharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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
			return;
		}
	}

	Super::PerformMovement(DeltaTime);
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
