// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FPGAGlobalTags.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAGlobalTags : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static const FGameplayTag& Relationship_Self()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Self")));
		return Tag;
	}

	static const FGameplayTag& Relationship_Friendly()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Friendly")));
		return Tag;
	}

	static const FGameplayTag& Relationship_Hostile()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Relationship.Hostile")));
		return Tag;
	}
};
