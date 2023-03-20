// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPTagRelationshipMapping.h"

#include "AbilitySystem/FPGAGameplayAbilitiesLibrary.h"
#include "HAL/Platform.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPTagRelationshipMapping)

// void UFPTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
// {
// 	// Simple iteration for now
// 	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
// 	{
// 		const FFPTagRelationship& Tags = AbilityTagRelationships[i];
// 		if (AbilityTags.HasTag(Tags.IdTag))
// 		{
// 			if (OutTagsToBlock)
// 			{
// 				OutTagsToBlock->AppendTags(Tags);
// 			}
// 			if (OutTagsToCancel)
// 			{
// 				OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
// 			}
// 		}
// 	}
// }

void UFPTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	// Simple iteration for now
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FFPTagRelationship& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.IdTag))
		{
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Tags.RequiredTags);
			}
			if (OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Tags.BlockedTags);
			}
		}
	}
}

// bool UFPTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
// {
// 	// Simple iteration for now
// 	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
// 	{
// 		const FFPTagRelationship& Tags = AbilityTagRelationships[i];
//
// 		if (Tags.IdTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
// 		{
// 			return true;
// 		}
// 	}
//
// 	return false;
// }

void UFPTagRelationshipMapping::GetTagQuery(const FGameplayTagContainer& AbilityTags, FGameplayTagQuery& OutTagQuery) const
{
	// FGameplayTagQueryExpression Expr;
	// OutTagQuery.GetQueryExpr(Expr);
	//
	// // Simple iteration for now
	// for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	// {
	// 	const FFPTagRelationship& Tags = AbilityTagRelationships[i];
	// 	if (AbilityTags.HasTag(Tags.AbilityTag))
	// 	{
	// 		Tags.TagQuery.GetQueryExpr();
	// 	}
	// }
}

void UFPTagRelationshipMapping::GetTagRelationships(const FGameplayTagContainer& AbilityTags, TArray<TSharedRef<FFPTagRelationship>>& OutTagRelationships)
{
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		FFPTagRelationship& Relationship = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Relationship.IdTag))
		{
			OutTagRelationships.Add(MakeShareable(&Relationship));
		}
	}
}

// void UFPTagRelationshipMapping::GetAllQueryTags(const FGameplayTagContainer& AbilityTags, TArray<FGameplayTag>& OutTagDictionary)
// {
// 	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
// 	{
// 		FFPTagRelationship& Relationship = AbilityTagRelationships[i];
// 		if (AbilityTags.HasTag(Relationship.AbilityTag))
// 		{
// 			UFPGAGameplayAbilitiesLibrary::GetQueryTags(Relationship.TagQuery, OutTagDictionary);
// 		}
// 	}
// }
