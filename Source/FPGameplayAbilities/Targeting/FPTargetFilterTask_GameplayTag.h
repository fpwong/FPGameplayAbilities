// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilter.h"
#include "GameplayEffectTypes.h"
#include "FPTargetFilterTask_GameplayTag.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="TargetFilterTask Gameplay Tag"))
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTask_GameplayTag : public FFPTargetFilterTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	FGameplayTagRequirements SourceRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	FGameplayTagRequirements TargetRequirements;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override;

	bool CheckTagRequirements(const AActor* Source, const AActor* Target, const FGameplayTagRequirements& Requirements, OUT FGameplayTagContainer* OutFailureTags) const;

	virtual void BindToChanges(FFPTargetFilterObserver& Observer, AActor* SourceActor, AActor* TargetActor) const override;

	void BindToActor(FFPTargetFilterObserver& Observer, AActor* Actor, const FGameplayTagRequirements& Requirements) const;
};

USTRUCT(BlueprintType, meta=(DisplayName="TargetFilterTask Relationship"))
struct FPGAMEPLAYABILITIES_API FFPTargetFilterTask_Relationship final : public FFPTargetFilterTask
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta=(Categories="Relationship"))
	FGameplayTagContainer Requirements;

	virtual bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor, OUT FGameplayTagContainer* OutFailureTags = nullptr) const override;

	// TODO bind to team changes 
};
