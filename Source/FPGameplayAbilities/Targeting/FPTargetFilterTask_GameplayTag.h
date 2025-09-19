﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterTask.h"
#include "FPTargetFilterObserver.h"
#include "GameplayEffectTypes.h"
#include "FPTargetFilterTask_GameplayTag.generated.h"

struct FFPTargetFilterTaskSetObserver;

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API UFPTargetFilterTask_GameplayTag final : public UFPTargetFilterTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Default)
	FGameplayTagRequirements SourceRequirements;

	UPROPERTY(EditAnywhere, Category = Default)
	FGameplayTagRequirements TargetRequirements;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override;

	// FGameplayTagContainer GetAllRelatedTags();

	bool CheckTagRequirements(const AActor* Source, const AActor* Target, const FGameplayTagRequirements& Requirements, OUT FGameplayTagContainer* OutFailureTags) const;

	virtual FFPTargetFilterObserver* MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;
};

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterObserver_GameplayTag : public FFPTargetFilterObserver
{
	GENERATED_BODY()

	virtual ~FFPTargetFilterObserver_GameplayTag() override;

	virtual void Init(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;

	void BindToActor(AActor* Actor, const FGameplayTagRequirements& Requirements, TMap<FGameplayTag, FDelegateHandle>& OutDelegateHandles);

	void UnbindDelegates(AActor* Actor, const TMap<FGameplayTag, FDelegateHandle>& DelegateHandles);

	void OnFilterTagChanged(FGameplayTag Tag, int NewCount);

private:
	// TMap<FGameplayTag, FDelegateHandle> TagChangedDelegates;
	TMap<FGameplayTag, FDelegateHandle> SourceDelegateHandles;
	TMap<FGameplayTag, FDelegateHandle> TargetDelegateHandles;
};

UCLASS(Blueprintable)
class FPGAMEPLAYABILITIES_API UFPTargetFilterTask_Relationship final : public UFPTargetFilterTask
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Default, meta=(Categories="Relationship"))
	FGameplayTagContainer Requirements;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override;

	// bool CheckTagRequirements(const AActor* Source, const AActor* Target, const FGameplayTagRequirements& Requirements, OUT FGameplayTagContainer* OutFailureTags) const;
	// virtual FFPTargetFilterObserver* MakeBinding(UFPTargetFilterTask* FilterTask, AActor* SourceActor, AActor* TargetActor) override;
};
