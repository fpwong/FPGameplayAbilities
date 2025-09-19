// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
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

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf = "Source", AutoCreateRefTerm="ActorsToIgnore,FilterTaskSet"))
	static TArray<AActor*> FPTraceActorsCapsule(AActor* Source,
										const FVector Start,
										const FVector End,
										const TArray<AActor*>& ActorsToIgnore,
										ETraceTypeQuery TraceChannel,
										float Radius,
										float HalfHeight,
										const FFPTargetFilterTaskSet& FilterTaskSet,
										EDrawDebugTrace::Type DrawDebugType);

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf = "Source", AutoCreateRefTerm="ActorsToIgnore,FilterTaskSet"))
	static TArray<AActor*> FPTraceActorsSphere(AActor* Source,
										const FVector Location,
										const TArray<AActor*>& ActorsToIgnore,
										ETraceTypeQuery TraceChannel,
										float Radius,
										const FFPTargetFilterTaskSet& FilterTaskSet,
										EDrawDebugTrace::Type DrawDebugType);

	UFUNCTION(BlueprintCallable)
	static TArray<AActor*> FilterActorArray(AActor* Source, const TArray<AActor*>& Actors, const FFPTargetFilterTaskSet& FilterTaskSet);
};
