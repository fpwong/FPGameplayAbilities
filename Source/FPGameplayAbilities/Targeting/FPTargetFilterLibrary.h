// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilterLibrary.generated.h"

struct FFPTargetFilterTaskSet;
struct FGameplayTagContainer;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPTargetFilterLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void GetAllRequiredTagsInTaskSet(	
		const FFPTargetFilterTaskSet& TaskSet,
		AActor* Source,
		AActor* Target, 
		const FGameplayTagContainer& Tags,
		OUT FGameplayTagContainer& OutTags);
};
