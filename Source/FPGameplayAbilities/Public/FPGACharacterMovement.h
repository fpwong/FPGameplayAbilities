// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPGACharacterMovement.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGACharacterMovement : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()
		
	class FSavedMove_My : public FSavedMove_Character
	{

	public:
		virtual void Clear() override;
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
		virtual void PrepMoveFor(ACharacter* Character) override;
		virtual uint8 GetCompressedFlags() const override;

		uint8 bSavedRequestMaxWalkSpeedChange : 1;
	};

	class FNetworkPredictionData_My : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_My(const UCharacterMovementComponent& ClientMovement);
		virtual FSavedMovePtr AllocateNewMove() override;
	};



public:
	//UFPGACharacterMovement();

	//virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	uint8 bRequestMaxWalkSpeedChange : 1;

	UFUNCTION(Unreliable, Server, WithValidation)
	void Server_SetMaxWalkSpeed(const float NewMaxWalkSpeed);

	float MyNewMaxWalkSpeed;

	UFUNCTION(BlueprintCallable, Category = "Max Walk Speed")
	void SetMaxWalkSpeed(float NewMaxWalkSpeed);
	void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
};


