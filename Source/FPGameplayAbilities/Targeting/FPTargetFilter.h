// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTypes.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "FPTargetFilter.generated.h"

struct FFPTargetFilterObserver;
struct FGameplayTagContainer;

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTask
{
	GENERATED_BODY()

	virtual ~FFPTargetFilterTask() = default;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Default)
	EFPTargetFilterTaskType TaskType = EFPTargetFilterTaskType::FailureTask;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const { return true; }

	virtual void BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const {}

	virtual void GetChildTasks(TArray<FInstancedStruct>& OutTasks) const {}
};

USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTask_Compound : public FFPTargetFilterTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/FPGameplayAbilities.FPTargetFilterTask", ExcludeBaseStruct))
	TArray<FInstancedStruct> Tasks;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override;

	virtual void BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const override;

	virtual void GetChildTasks(TArray<FInstancedStruct>& OutTasks) const override { OutTasks.Append(Tasks); }
};

USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FPGAMEPLAYABILITIES_API FFPTargetFilter
{
	GENERATED_BODY()

	FFPTargetFilter() = default;

	FFPTargetFilter(const FFPTargetFilterTask& InTask);

	UPROPERTY(EditAnywhere, meta = (BaseStruct = "/Script/FPGameplayAbilities.FPTargetFilterTask", ExcludeBaseStruct))
	FInstancedStruct Task;

	bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const;

	const FFPTargetFilterTask& GetTask() const;

	TArray<FInstancedStruct> GetAllTasks();

	bool IsValid() const;
};