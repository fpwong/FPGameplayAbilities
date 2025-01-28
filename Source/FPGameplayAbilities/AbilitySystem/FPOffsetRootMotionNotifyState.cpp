// Fill out your copyright notice in the Description page of Project Settings.


#include "FPOffsetRootMotionNotifyState.h"

UFPOffsetRootMotionNotifyState::UFPOffsetRootMotionNotifyState()
{
	bIsNativeBranchingPoint = true;

#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif // WITH_EDITORONLY_DATA
}

void UFPOffsetRootMotionNotifyState::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyBegin(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			AnimInstance->OnPlayMontageNotifyBegin.Broadcast(FName("FPRootMotionNotify"), BranchingPointPayload);
		}
	}
}

void UFPOffsetRootMotionNotifyState::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotifyEnd(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
		{
			AnimInstance->OnPlayMontageNotifyEnd.Broadcast(FName("FPRootMotionNotify"), BranchingPointPayload);
		}
	}
}
