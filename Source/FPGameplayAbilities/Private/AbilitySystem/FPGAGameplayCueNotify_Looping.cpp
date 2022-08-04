// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/FPGAGameplayCueNotify_Looping.h"

#include "GameFramework/Character.h"

FGameplayCueParameters AFPGAGameplayCueNotify_Looping::MakeModifiedParameters(AActor* Target, const FGameplayCueParameters& Parameters)
{
	FGameplayCueParameters Modified = Parameters;
	const float TargetHeight = Target->GetSimpleCollisionHalfHeight();
	Modified.Location = FVector(0, 0, TargetHeight * RelativeHeightOffset);
	return Modified;
}

bool AFPGAGameplayCueNotify_Looping::OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	return Super::OnActive_Implementation(Target, bUseRelativeHeightOffset ? MakeModifiedParameters(Target, Parameters) : Parameters);
}

bool AFPGAGameplayCueNotify_Looping::WhileActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	return Super::WhileActive_Implementation(Target, bUseRelativeHeightOffset ? MakeModifiedParameters(Target, Parameters) : Parameters);
}

bool AFPGAGameplayCueNotify_Looping::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	return Super::OnExecute_Implementation(Target, bUseRelativeHeightOffset ? MakeModifiedParameters(Target, Parameters) : Parameters);
}
