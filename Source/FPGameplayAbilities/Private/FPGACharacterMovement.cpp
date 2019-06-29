// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGACharacterMovement.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/FPGAAttributeSet.h"
#include "GameFramework/Character.h"

/////////////////////////////////////////////////

void UFPGACharacterMovement::FSavedMove_My::Clear()
{
	FSavedMove_Character::Clear();
	bSavedRequestMaxWalkSpeedChange = false;
}

void UFPGACharacterMovement::FSavedMove_My::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UFPGACharacterMovement* CharacterMovement = Cast<UFPGACharacterMovement>(Character->GetCharacterMovement());
	if (CharacterMovement != nullptr)
	{
		bSavedRequestMaxWalkSpeedChange = CharacterMovement->bRequestMaxWalkSpeedChange;
	}
}

bool UFPGACharacterMovement::FSavedMove_My::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	if (bSavedRequestMaxWalkSpeedChange != ((FSavedMove_My*)&NewMove)->bSavedRequestMaxWalkSpeedChange)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UFPGACharacterMovement::FSavedMove_My::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);
	UFPGACharacterMovement* CharacterMovement = Cast<UFPGACharacterMovement>(Character->GetCharacterMovement());
}

uint8 UFPGACharacterMovement::FSavedMove_My::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();
	if (bSavedRequestMaxWalkSpeedChange)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

/////////////////////////////////////////////////


UFPGACharacterMovement::FNetworkPredictionData_My::FNetworkPredictionData_My(const UCharacterMovementComponent& ClientMovement)
	: FNetworkPredictionData_Client_Character(ClientMovement)
{
}

FSavedMovePtr UFPGACharacterMovement::FNetworkPredictionData_My::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_My());
}

//////////////////////////////////////////////////


UFPGACharacterMovement::UFPGACharacterMovement(const class FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{

}

FNetworkPredictionData_Client* UFPGACharacterMovement::GetPredictionData_Client() const
{
	//check(PawnOwner != nullptr)
	//check(PawnOwner->Role < ROLE_Authority)

	if (!ClientPredictionData)
	{
		UFPGACharacterMovement* MutableThis = const_cast<UFPGACharacterMovement*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_My(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}


//float UFPGACharacterMovement::GetMaxSpeed() const
//{
//	//UAbilitySystemComponent* AbilitySystem = Cast<UAbilitySystemComponent>(PawnOwner->FindComponentByClass(UAbilitySystemComponent::StaticClass()));
//	//if (AbilitySystem)
//	//{
//	//	return AbilitySystem->GetNumericAttribute(UFPGAAttributeSet::GetMovementSpeedAttribute());
//	//}
//
//	return Super::GetMaxSpeed();
//}


void UFPGACharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	UCharacterMovementComponent::UpdateFromCompressedFlags(Flags);
	
	bRequestMaxWalkSpeedChange = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UFPGACharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	UCharacterMovementComponent::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (!CharacterOwner)
		return;

	if (bRequestMaxWalkSpeedChange)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Actually changed the max walk speed to %f"), MyNewMaxWalkSpeed);

		bRequestMaxWalkSpeedChange = false;
		MaxWalkSpeed = MyNewMaxWalkSpeed;
	}
}

void UFPGACharacterMovement::SetMaxWalkSpeed(float NewMaxWalkSpeed)
{
	if (PawnOwner->IsLocallyControlled())
	{
		MyNewMaxWalkSpeed = NewMaxWalkSpeed;
		Server_SetMaxWalkSpeed(NewMaxWalkSpeed);
	}

	bRequestMaxWalkSpeedChange = true;
}

void UFPGACharacterMovement::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);

	if (Acceleration.IsZero())
	{
		// Stop movement
		Velocity = FVector::ZeroVector;
	}
	else
	{
		// Set velocity to max speed in the direction of acceleration (ignoring magnitude)
		Velocity = GetMaxSpeed() * Acceleration.GetSafeNormal();
	}
}

void UFPGACharacterMovement::Server_SetMaxWalkSpeed_Implementation(const float InNewMaxWalkSpeed)
{
	MyNewMaxWalkSpeed = InNewMaxWalkSpeed;
}

bool UFPGACharacterMovement::Server_SetMaxWalkSpeed_Validate(const float NewMaxWalkSpeed)
{
	return true;
}
