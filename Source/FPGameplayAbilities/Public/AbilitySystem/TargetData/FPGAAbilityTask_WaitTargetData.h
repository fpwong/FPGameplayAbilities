// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTargetActor.h"
//#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FPGAGameplayAbilityTargetActor.h"
#include "FPGAAbilityTask_WaitTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPGAWaitTargetDataDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
 * Wait for targeting actor (spawned from parameter) to provide data. Can be set not to end upon outputting data. Can be ended by task name.
 *
 * WARNING: These actors are spawned once per ability activation and in their default form are not very efficient
 * For most games you will need to subclass and heavily modify this actor, or you will want to implement similar functions in a game-specific actor or blueprint to avoid actor spawn costs
 * This task is not well tested by internal games, but it is a useful class to look at to learn how target replication occurs
 */
UCLASS(notplaceable)
class FPGAMEPLAYABILITIES_API UFPGAAbilityTask_WaitTargetData: public UAbilityTask
{
	GENERATED_BODY()

public:
	UFPGAAbilityTask_WaitTargetData(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FFPGAWaitTargetDataDelegate	ValidData;

	UPROPERTY(BlueprintAssignable)
	FFPGAWaitTargetDataDelegate	Cancelled;

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	UFUNCTION()
	void OnTargetDataReplicatedCancelledCallback();

	UFUNCTION()
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data);

	/** Spawns target actor and waits for it to return valid data or to be canceled. */
	UFUNCTION(BlueprintCallable, meta=(HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms="Instigator"), Category="Ability|Tasks")
	static UFPGAAbilityTask_WaitTargetData* FPGAWaitTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, TSubclassOf<AFPGAGameplayAbilityTargetActor> Class);

	/** Uses specified target actor and waits for it to return valid data or to be canceled. */
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UFPGAAbilityTask_WaitTargetData* FPGAWaitTargetDataUsingActor(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AFPGAGameplayAbilityTargetActor* TargetActor);

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	bool BeginSpawningActor(UGameplayAbility* OwningAbility, TSubclassOf<AFPGAGameplayAbilityTargetActor> Class, AFPGAGameplayAbilityTargetActor*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	void FinishSpawningActor(UGameplayAbility* OwningAbility, AFPGAGameplayAbilityTargetActor* SpawnedActor);

	/** Called when the ability is asked to confirm from an outside node. What this means depends on the individual task. By default, this does nothing other than ending if bEndTask is true. */
	virtual void ExternalConfirm(bool bEndTask) override;

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

protected:
	bool ShouldSpawnTargetActor() const;
	void InitializeTargetActor(AFPGAGameplayAbilityTargetActor* SpawnedActor);
	void FinalizeTargetActor(AFPGAGameplayAbilityTargetActor* SpawnedActor) const;

	void RegisterTargetDataCallbacks();

	virtual void OnDestroy(bool AbilityEnded) override;

	bool ShouldReplicateDataToServer() const;

	UPROPERTY()
	TSubclassOf<AFPGAGameplayAbilityTargetActor> TargetClass;

	/** The TargetActor that we spawned */
	UPROPERTY()
	AFPGAGameplayAbilityTargetActor* TargetActor;

	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;

	FDelegateHandle OnTargetDataReplicatedCallbackDelegateHandle;
};
