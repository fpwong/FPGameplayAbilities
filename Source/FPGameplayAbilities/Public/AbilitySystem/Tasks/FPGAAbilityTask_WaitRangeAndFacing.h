// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "FPGACharacter.h"
#include "AITypes.h"
#include "FPGAAbilityTask_WaitRangeAndFacing.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitRangeAndFacingDelegate);

UENUM()
enum class EWaitRangeAndFacingExecution : uint8
{
	Reset,
};

///** Enum used to indicate desired behavior for MoveComponentTo latent function. */
//UENUM()
//namespace EMoveComponentAction
//{
//	enum Type
//	{
//		/** Move to target over specified time. */
//		Move, 
//		/** If currently moving, stop. */
//		Stop,
//		/** If currently moving, return to where you started, over the time elapsed so far. */
//		Return
//	};
//}

UCLASS()
	class FPGAMEPLAYABILITIES_API UFPGAAbilityTask_WaitRangeAndFacing : public UAbilityTask_WaitCancel
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitRangeAndFacingDelegate OnFinish;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float Range;

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", ExpandEnumAsExecs = "ResetExec"))
	static UFPGAAbilityTask_WaitRangeAndFacing* WaitRangeAndFacing(
		TEnumAsByte<EWaitRangeAndFacingExecution> ResetExec,
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		float MaxRange,
		FVector Location,
		AActor* Actor);

private:
	void SetOwnerCharacter();

	void TickTask(float DeltaTime) override;

	void Activate() override;

	void OnDestroy(bool AbilityEnding) override;

	void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;
};
