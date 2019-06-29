// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "FPGACharacter.h"
#include "AITypes.h"
#include "FPGAAbilityTask_MoveAndFace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitRangeFacingDelegate);

UCLASS()
	class FPGAMEPLAYABILITIES_API UFPGAAbilityTask_MoveAndFace : public UAbilityTask_WaitCancel //public UAbilityTask//
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitRangeFacingDelegate OnFinish;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float Range;

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UFPGAAbilityTask_MoveAndFace* MoveAndFace(UGameplayAbility* OwningAbility, FName TaskInstanceName, float MaxRange, FVector Location, AActor* Actor);

private:
	class UNavigationSystemV1* NavSys;

	class UPathFollowingComponent* PFollowComp;

	FAIMoveRequest MoveRequest;

	FVector NavLocation;

	bool bReachedGoal = false;

	void SetOwnerCharacter();

	void TickTask(float DeltaTime) override;

	void Activate() override;

	void OnDestroy(bool AbilityEnding) override;

	void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;

	void StartMoving();

	FVector GetDesiredLocation();
};
