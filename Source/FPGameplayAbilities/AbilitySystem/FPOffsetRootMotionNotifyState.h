// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameFramework/RootMotionSource.h"
#include "FPOffsetRootMotionNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPOffsetRootMotionNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

	UFPOffsetRootMotionNotifyState();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector RelativeOffset = FVector(100, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bRestrictSpeed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	bool bSetMovementMode = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TEnumAsByte<EMovementMode> MovementMode = MOVE_Walking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	TObjectPtr<UCurveVector> PathOffsetCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	ERootMotionFinishVelocityMode FinishVelocityMode = ERootMotionFinishVelocityMode::SetVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	FVector FinishSetVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float FinishClampVelocity = 0.0f;

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload) override;

	uint16 RootMotionSourceID;
};
