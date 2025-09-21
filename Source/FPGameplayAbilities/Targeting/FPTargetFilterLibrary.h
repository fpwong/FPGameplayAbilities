// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPTargetFilter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FPTargetFilterLibrary.generated.h"

struct FFPTargetFilterTask_Preset;
struct FFPTargetFilterTask_AttributeComparison;
struct FFPTargetFilterTask_Relationship;
struct FFPTargetFilterTask_GameplayTag;
struct FFPTargetFilterTaskSet;
struct FGameplayTagContainer;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPTargetFilterLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf = "Source", AutoCreateRefTerm="ActorsToIgnore,FilterTaskSet"))
	static TArray<AActor*> FPTraceActorsCapsule(AActor* Source,
										const FVector Start,
										const FVector End,
										const TArray<AActor*>& ActorsToIgnore,
										ETraceTypeQuery TraceChannel,
										float Radius,
										float HalfHeight,
										const FFPTargetFilter& FilterTaskSet,
										EDrawDebugTrace::Type DrawDebugType);

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf = "Source", AutoCreateRefTerm="ActorsToIgnore,FilterTaskSet"))
	static TArray<AActor*> FPTraceActorsSphere(AActor* Source,
										const FVector Location,
										const TArray<AActor*>& ActorsToIgnore,
										ETraceTypeQuery TraceChannel,
										float Radius,
										const FFPTargetFilter& FilterTaskSet,
										EDrawDebugTrace::Type DrawDebugType);

	UFUNCTION(BlueprintCallable)
	static TArray<AActor*> FilterActorArray(AActor* Source, const TArray<AActor*>& Actors, const FFPTargetFilter& FilterTaskSet);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To FilterTask", CompactNodeTitle = "->", BlueprintAutocast), Category = "Conversion")
	static FFPTargetFilter Conv_TaskGameplayTag(const FFPTargetFilterTask_GameplayTag& FilterTask);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To FilterTask", CompactNodeTitle = "->", BlueprintAutocast), Category = "Conversion")
	static FFPTargetFilter Conv_TaskRelationship(const FFPTargetFilterTask_Relationship& FilterTask);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To FilterTask", CompactNodeTitle = "->", BlueprintAutocast), Category = "Conversion")
	static FFPTargetFilter Conv_TaskAttributeComparison(const FFPTargetFilterTask_AttributeComparison& FilterTask);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To FilterTask", CompactNodeTitle = "->", BlueprintAutocast), Category = "Conversion")
	static FFPTargetFilter Conv_TaskPreset(const FFPTargetFilterTask_Preset& FilterTask);

	UFUNCTION(BlueprintCallable)
	static bool EvaluateFilterTask(const FFPTargetFilter& FilterTask, AActor* SourceActor, AActor* TargetActor);
};
