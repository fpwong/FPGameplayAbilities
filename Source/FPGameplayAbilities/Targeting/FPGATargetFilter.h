// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/TargetData/FPGAGameplayTargetDataFilter.h"
#include "FPGATargetFilter.generated.h"

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGATargetFilter
{
	GENERATED_BODY()

public:
	// FFPGATargetFilter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TargetFilter)
	FGameplayTagQuery TargetTagQuery = FGameplayTagQuery::EmptyQuery;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TargetFilter)
	FGameplayEffectQuery RequiredEffectQuery;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TargetFilter)
	FGameplayEffectQuery BlockedEffectQuery;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TargetFilter)
	FAISenseAffiliationFilter Affiliation;

	bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const;
};

DECLARE_MULTICAST_DELEGATE(FFPOnTargetFilterFailed);

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGATargetFilterValidation
{
	GENERATED_BODY()

	FFPGATargetFilter Filter;

	UPROPERTY()
	AActor* SourceActor = nullptr;

	UPROPERTY()
	AActor* TargetActor = nullptr;

	FFPOnTargetFilterFailed OnFilterFailed;

	void BindToActor(const FFPGATargetFilter& InFilter, AActor* InSourceActor, AActor* InTargetActor);

	void OnFilterTagChanged(FGameplayTag Tag, int NewCount);

	void Clear();

private:
	TMap<FGameplayTag, FDelegateHandle> TagChangedDelegates;
};
