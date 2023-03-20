// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTask.h"
#include "FPTargetFilterObserver.h"

#include "GameplayEffectTypes.h"
#include "FPTargetFilterTask_FilterPreset.generated.h"

class UFPTargetFilterPreset;
struct FFPTargetFilterTaskSetObserver;

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API UFPTargetFilterTask_FilterPreset final : public UFPTargetFilterTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Default)
	UFPTargetFilterPreset* Preset;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const override;

	virtual FFPTargetFilterObserver* MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;
};

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterObserver_FilterPreset : public FFPTargetFilterObserver
{
	GENERATED_BODY()

	void InitFilterPreset(UFPTargetFilterTask_FilterPreset* Task, AActor* SourceActor, AActor* TargetActor);

	void HandleResultChanged(AActor*, bool);

private:
	TSharedPtr<FFPTargetFilterTaskSetObserver> TaskSetObserver;
};
