// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FPGAAbilityTask_WaitCancel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFPGAWaitCancelDelegate);

UCLASS()
	class FPGAMEPLAYABILITIES_API UFPGAAbilityTask_WaitCancel : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FFPGAWaitCancelDelegate OnCancel;

	UFUNCTION()
	void OnCancelCallback();

	UFUNCTION()
	void OnLocalCancelCallback();

	UFUNCTION(BlueprintCallable, meta=(HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", DisplayName="FPGA Wait for Cancel Input"), Category=
		"Ability|Tasks")
	static UFPGAAbilityTask_WaitCancel* FPGAWaitCancel(UGameplayAbility* OwningAbility, bool bEndOnCancelInput = true);

	virtual void Activate() override;

protected:

	bool RegisteredCallbacks;

	bool bEndOnCancelInput;

	virtual void OnDestroy(bool AbilityEnding) override;
};
