// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "FPGameplayAbilityTargetActor_CursorTrace.generated.h"

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API AFPGameplayAbilityTargetActor_CursorTrace : public AGameplayAbilityTargetActor_Trace
{
	GENERATED_UCLASS_BODY()

protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) override;

	bool GetHitResultsUnderCursor(TArray<FHitResult>& OutHitResults);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true), Replicated, Category = Targeting)
	float HitResultTraceDistance;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true), Replicated, Category = Targeting)
	TEnumAsByte<ECollisionChannel> DefaultClickTraceChannel;

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true), Replicated, Category = Targeting)
	TEnumAsByte<ETraceTypeQuery> GroundTraceChannel;
};
