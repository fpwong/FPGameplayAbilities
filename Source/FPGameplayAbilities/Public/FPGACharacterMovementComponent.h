// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPGACharacterMovementComponent.generated.h"

//////////////////////////////////////////////////////////////////////////
// Client to Server movement data
//////////////////////////////////////////////////////////////////////////

/**
 * FCharacterNetworkMoveData encapsulates a client move that is sent to the server for UCharacterMovementComponent networking.
 *
 * Adding custom data to the network move is accomplished by deriving from this struct, adding new data members, implementing ClientFillNetworkMoveData(), implementing Serialize(), 
 * and setting up the UCharacterMovementComponent to use an instance of a custom FCharacterNetworkMoveDataContainer (see that struct for more details).
 * 
 * @see FCharacterNetworkMoveDataContainer
 */

struct FPGAMEPLAYABILITIES_API FFPGACharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
};

//////////////////////////////////////////////////////////////////////////
// Server to Client response
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/**
 * Response from the server to the client about a move that is being acknowledged.
 * Internally it mainly copies the FClientAdjustment from the UCharacterMovementComponent indicating the response, as well as
 * setting a few relevant flags about the response and serializing the response to and from an FArchive for handling the variable-size
 * payload over the network.
 */
struct FPGAMEPLAYABILITIES_API FFPGACharacterMoveResponseDataContainer : public FCharacterMoveResponseDataContainer
{
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPGAMEPLAYABILITIES_API UFPGACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer BlockedMovementTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayAttribute MovementSpeedAttribute;

	// Sets default values for this component's properties
	UFPGACharacterMovementComponent();

	virtual void BeginPlay() override;

	// ~~ CharacterMovementComponent
	virtual float GetMaxSpeed() const override;
	virtual void ServerMoveHandleClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;
	virtual bool ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;
	virtual void PerformMovement(float DeltaTime) override;
	// ~~ CharacterMovementComponent

protected:
	UAbilitySystemComponent* AbilitySystem;
};
