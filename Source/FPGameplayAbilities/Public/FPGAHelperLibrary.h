// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FPGameplayAbilities/Targeting/FPGATargetType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Perception/AISense.h"
#include "FPGAHelperLibrary.generated.h"

struct FGameplayTagCountContainer;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AIPerception")
	static bool CheckAffiliation(FAISenseAffiliationFilter Affiliation, AActor* ActorA, AActor* ActorB);

	static bool IsTargetTypeFlagChecked(int32 TargetTypeFlags, int32 InFlag)
	{
		return (TargetTypeFlags & InFlag) != 0;
	}

	static bool IsTargetTypeFlagChecked(int32 TargetTypeFlags, EFPGATargetTypeFlags InFlag)
	{
		return IsTargetTypeFlagChecked(TargetTypeFlags, StaticCast<int32>(InFlag));
	}

	static TMap<FGameplayTag, int32> GetTagCountMapping(const FGameplayTagCountContainer& TagCountContainer);
};
