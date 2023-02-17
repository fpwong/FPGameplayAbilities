// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGATargetData.h"
// #include "FPGATargetingRequest.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFPTargetingEnd, FGameplayAbilityTargetDataHandle);

//USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPGATargetingRequest
{
	//GENERATED_BODY()

private:
	FFPGATargetingRequest() = default;

public:
	static FFPGATargetingRequest MakeTargetingRequest(
		const FFPGATargetingRequirements& TargetingRequirements,
		AActor* InRequestingActor);

	FSimpleMulticastDelegate OnRequestCompleted;

	// UPROPERTY()
	FFPGATargetingRequirements Requirements;

	// UPROPERTY()
	TWeakObjectPtr<AActor> RequestingActor;

	// UPROPERTY()
	FGameplayAbilityTargetDataHandle TargetData;

	bool IsValidEventData() const;

	bool DoesRequireTargeting() const;

	/* Try building target data from the first hit result which satisfies the current stage */
	bool TryApplyingHitResults(const TArray<FHitResult>& HitResults);

	/* Try building target data if this hit result satisfies the current stage */
	bool TryApplyingHitResult(const FHitResult& HitResult);

	const FFPGATargetingStage& GetCurrentStage() const;
};
