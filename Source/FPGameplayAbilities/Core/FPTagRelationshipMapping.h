// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Containers/Array.h"
#include "Engine/DataAsset.h"
#include "FPTagRelationshipMapping.generated.h"

class UObject;

/** Struct that defines the relationship between different ability tags */
USTRUCT()
struct FFPTagRelationship
{
	GENERATED_BODY()

	/** The tag that this container relationship is about. Single tag, but abilities can have multiple of these */
	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "TagGroup"))
	FGameplayTag IdTag;

	// /** The other ability tags that will be blocked by any ability using this tag */
	// UPROPERTY(EditAnywhere, Category = Ability)
	// FGameplayTagContainer AbilityTagsToBlock;
	//
	// /** The other ability tags that will be canceled by any ability using this tag */
	// UPROPERTY(EditAnywhere, Category = Ability)
	// FGameplayTagContainer AbilityTagsToCancel;

	/** If an ability has the tag, this is implicitly added to the activation required tags of the ability */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer RequiredTags;

	/** If an ability has the tag, this is implicitly added to the activation blocked tags of the ability */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer BlockedTags;

	// UPROPERTY(EditAnywhere, Category = Ability)
	// FGameplayTagQuery TagQuery;
};

/** Mapping of how ability tags block or cancel other abilities */
UCLASS()
class UFPTagRelationshipMapping final : public UDataAsset
{
	GENERATED_BODY()

private:
	/** The list of relationships between different gameplay tags (which ones block or cancel others) */
	UPROPERTY(EditAnywhere, Category = Ability, meta=(TitleProperty="IdTag"))
	TArray<FFPTagRelationship> AbilityTagRelationships;

public:
	/** Given a set of ability tags, parse the tag relationship and fill out tags to block and cancel */
	// void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

	/** Given a set of ability tags, add additional required and blocking tags */
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

	/** Returns true if the specified ability tags are canceled by the passed in action tag */
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;

	void GetTagQuery(const FGameplayTagContainer& AbilityTags, FGameplayTagQuery& OutTagQuery) const;

	void GetTagRelationships(const FGameplayTagContainer& AbilityTags, TArray<TSharedRef<FFPTagRelationship>>& OutTagRelationships);

	// void GetAllQueryTags(const FGameplayTagContainer& AbilityTags, TArray<FGameplayTag>& OutTagDictionary);
};
