// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTaskSet.h"
#include "FPTargetFilterPreset.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API UFPTargetFilterPreset final : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Default, meta = (ShowOnlyInnerProperties))
	FFPTargetFilterTaskSet TaskSet;

	bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const
	{
		return TaskSet.DoesFilterPass(SourceActor, TargetActor);
	};
};
