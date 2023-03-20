// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterPreset.h"
#include "GameplayTagContainer.h"
#include "FPGATargetFilter.generated.h"

class UFPTagRelationshipMapping;
class UFPTargetFilterPreset;
struct FFPTargetFilterTaskSetObserver;

UENUM()
enum class EFPFilterValidationEvent : uint8
{
	Valid,
	Invalid
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FFPFilterResultChanged, AActor*, bool);

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGATargetFilter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = TargetFilter)
	TArray<UFPTargetFilterPreset*> TargetingPresets;

	UPROPERTY(EditAnywhere, Category = TargetFilter)
	FFPTargetFilterTaskSet TargetFilterTaskSet;

	UPROPERTY(EditAnywhere, Category = TargetFilter)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, Category = TargetFilter)
	FGameplayTagContainer BlockedTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TargetFilter, meta=(Categories="TagGroup"))
	FGameplayTagContainer TagGroups;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TargetFilter)
	UFPTagRelationshipMapping* TagRelationshipMapping = nullptr;

	void AddAdditionalTags(const FGameplayTagContainer& AdditionalTags);

	bool DoesFilterPass(const AActor* SourceActor, const AActor* TargetActor) const;

	void PrintFilter() const;

	FGameplayTagContainer GetAllRelatedTags();
};

USTRUCT()
struct FPGAMEPLAYABILITIES_API FFPTargetFilterValidation
{
	GENERATED_BODY()
public:

	~FFPTargetFilterValidation() { Clear(); }

	FFPGATargetFilter Filter;

	UPROPERTY()
	TWeakObjectPtr<AActor> SourceActor;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> TargetsActor;

	// UPROPERTY()
	FFPFilterResultChanged OnFilterResultChanged;

	TArray<TSharedPtr<FFPTargetFilterTaskSetObserver>> PresetBindings;

	void Bind(const FFPGATargetFilter& InFilter, AActor* InSourceActor, AActor* InTargetActor);

	void Clear();

	bool DoesTargetPass() const { return bCurrentResult; }

	void HandleResultChanged(AActor* Actor, bool bNewResult);

	FFPFilterResultChanged OnResultChanged;

private:
	bool bCurrentResult = false;
};
