// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilter.h"
#include "FPTargetFilterPreset.generated.h"

UCLASS(BlueprintType)
class FPGAMEPLAYABILITIES_API UFPTargetFilterPreset final : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Default)
	FFPTargetFilter TargetFilter;

	UFUNCTION(BlueprintPure)
	static const FFPTargetFilter& GetTargetFilterFromPreset(UFPTargetFilterPreset* Preset) { return Preset->TargetFilter; }
};

USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTask_Preset : public FFPTargetFilterTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UFPTargetFilterPreset* Preset;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override
	{
		if (!Preset)
		{
			return true;
		}

		return Preset->TargetFilter.GetTask().DoesFilterPass(SourceActor, TargetActor, OutFailureTags);
	}

	virtual void BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const override
	{
		if (!Preset)
		{
			return;
		}

		Preset->TargetFilter.GetTask().BindToChanges(Observer, SourceActor, TargetActor);
	}

	virtual void GetChildTasks(TArray<FInstancedStruct>& OutTasks) const override
	{
		if (!Preset)
		{
			return;
		}

		Preset->TargetFilter.GetTask().GetChildTasks(OutTasks);
	}
};
