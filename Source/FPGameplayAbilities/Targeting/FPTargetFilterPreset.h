// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTaskSet.h"
#include "FPTargetFilterPreset.generated.h"

UCLASS(BlueprintType)
class FPGAMEPLAYABILITIES_API UFPTargetFilterPreset final : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Default, meta = (ShowOnlyInnerProperties))
	FFPTargetFilterTaskSet TaskSet;

	UFUNCTION(BlueprintCallable, Category = TargetFilter)
	bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
	{
		return TaskSet.DoesFilterPass(SourceActor, TargetActor);
	}

	UFUNCTION(BlueprintCallable, Category = TargetFilter)
	static bool EvaluateTargetFilter(UFPTargetFilterPreset* TargetFilterPreset, const AActor* SourceActor, const AActor* TargetActor)
	{
		return TargetFilterPreset ? TargetFilterPreset->DoesFilterPass(SourceActor, TargetActor) : false;
	}
};
