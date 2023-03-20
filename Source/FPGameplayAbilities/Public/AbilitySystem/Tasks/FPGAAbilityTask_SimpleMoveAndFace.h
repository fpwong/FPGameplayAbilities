// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "AITypes.h"
#include "FPGAAbilityTask_SimpleMoveAndFace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleMoveAndFaceDelegate);

UCLASS()
	class FPGAMEPLAYABILITIES_API UFPGAAbilityTask_SimpleMoveAndFace : public UAbilityTask_WaitCancel
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FSimpleMoveAndFaceDelegate OnFinish;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	float Range;

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	bool bEndOnReachingTarget;

	UPROPERTY()
	bool bHasFired;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UFPGAAbilityTask_SimpleMoveAndFace* SimpleMoveAndFace(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName = FName("MovementTask"),
		float MaxRange = 100.f,
		FVector Location = FVector(0.f),
		AActor* Target = nullptr,
		bool bEndOnReaching = true);

private:
	void SetOwnerCharacter();

	virtual void TickTask(float DeltaTime) override;

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityEnding) override;

	virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;
};
