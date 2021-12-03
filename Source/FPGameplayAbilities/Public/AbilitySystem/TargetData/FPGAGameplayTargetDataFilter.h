// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "Perception/AISense.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "FPGAGameplayTargetDataFilter.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FFPGATargetDataFilterCustomMatch, const AActor*);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FFPGATargetDataFilterCustomMatch_Dynamic, const AActor*, Actor, bool&, bReturnValue);

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGAGameplayTargetDataFilter
{
	GENERATED_USTRUCT_BODY()

public:
	FFPGAGameplayTargetDataFilter();

	// /** Actor we're comparing against. */
	// UPROPERTY()
	// AActor* SelfActor;

	/** Native delegate for providing custom matching conditions. */
	FFPGATargetDataFilterCustomMatch CustomMatchDelegate;

	/** BP-exposed delegate for providing custom matching conditions. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFPGATargetDataFilterCustomMatch_Dynamic CustomMatchDelegate_BP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer OwnerTags;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagQuery OwnerTagQuery;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayEffectQuery RequiredEffectQuery;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayEffectQuery BlockedEffectQuery;

	// UPROPERTY(BlueprintReadWrite, EditAnywhere)
	// FAISenseAffiliationFilter Affiliation;

	/** Initializes SelfActor */
	void InitializeFilterContext(AActor* FilterActor) {} //SelfActor = FilterActor; }

	bool FilterPassesForActor(const AActor* ActorToBeFiltered) const;
};
