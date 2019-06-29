// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FPGAAbilityTask_WaitCancel.h"
#include "FPGAAbilityTask_PlayMontageNotify.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMontageNotifyDelegate, FName, NotifyName);

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAAbilityTask_PlayMontageNotify : public UFPGAAbilityTask_WaitCancel
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FMontageNotifyDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FMontageNotifyDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FMontageNotifyDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FMontageNotifyDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FMontageNotifyDelegate OnNotifyBegin;

	UPROPERTY(BlueprintAssignable)
	FMontageNotifyDelegate OnNotifyEnd;

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "FPGAPlayMontageNotify",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UFPGAAbilityTask_PlayMontageNotify* CreatePlayMontageAndWaitProxy(
		UGameplayAbility* OwningAbility = nullptr,
		FName TaskInstanceName = FName("PlayMontageNotify"), 
		UAnimMontage* MontageToPlay = nullptr, 
		float Rate = 1.f, 
		FName StartSection = NAME_None, 
		bool bStopWhenAbilityEnds = true,
		bool bEndOnCancelInput = false,
		float AnimRootMotionTranslationScale = 1.f);



	virtual void Activate() override;

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

	virtual FString GetDebugString() const override;

public:
	//~ Begin UObject Interface
	virtual void BeginDestroy() override;
	//~ End UObject Interface

private:
	TWeakObjectPtr<UAnimInstance> AnimInstancePtr;

	virtual void OnDestroy(bool AbilityEnded) override;

	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingMontage();

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle InterruptedHandle;

	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	void UnbindDelegates();
};