// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilterTaskSetObserver.h"
#include "FPFilteredActorArray.generated.h"

USTRUCT()
struct FFPFilteredActorObserver
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY()
	FFPTargetFilterTaskSetObserver OngoingFilterTaskset;

	FFPFilteredActorObserver() = default;
	FFPFilteredActorObserver(const FFPTargetFilterTaskSet& Taskset, AActor* Source, AActor* Target);
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPFilteredActorArray
{
	GENERATED_BODY()

	void AddActor(AActor* Actor);

	void AddActors(const TArray<AActor*>& Actors);

	void ReplaceWithActors(const TArray<AActor*>& Actors);

	void ClearActors();

	void RemoveActor(AActor* Actor);

	const TArray<AActor*>& GetFilteredActors() const { return FilteredActors; }

	UPROPERTY(EditDefaultsOnly)
	FFPTargetFilterTaskSet FilterTaskset;

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> SourceActor;

	TMap<TWeakObjectPtr<AActor>, TSharedRef<FFPFilteredActorObserver>> AllActors;

protected:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> FilteredActors;

	void OnFilterResultChanged(AActor* Target, bool bResult);
};

UCLASS()
class UFilteredActorArrayHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FilteredActorArray")
	static void AddActor(UPARAM(ref) FFPFilteredActorArray& Array, AActor* Actor) { Array.AddActor(Actor); }

	UFUNCTION(BlueprintCallable, Category = "FilteredActorArray")
	static void AddActors(UPARAM(ref) FFPFilteredActorArray& Array, const TArray<AActor*>& Actors)  { Array.AddActors(Actors); }

	UFUNCTION(BlueprintCallable, Category = "FilteredActorArray")
	static void ReplaceWithActors(UPARAM(ref) FFPFilteredActorArray& Array, const TArray<AActor*>& Actors)  { Array.ReplaceWithActors(Actors); }

	UFUNCTION(BlueprintCallable, Category = "FilteredActorArray")
	static void ClearActors(UPARAM(ref) FFPFilteredActorArray& Array)  { Array.ClearActors(); }

	UFUNCTION(BlueprintCallable, Category = "FilteredActorArray")
	static void RemoveActor(UPARAM(ref) FFPFilteredActorArray& Array, AActor* Actor)  { Array.RemoveActor(Actor); }

	UFUNCTION(BlueprintCallable)
	static const TArray<AActor*>& GetFilteredActors(UPARAM(ref) FFPFilteredActorArray& Array);

	UFUNCTION(BlueprintCallable)
	static const FFPFilteredActorArray& GetFilteredActorsFromMap(const TMap<UObject*, FFPFilteredActorArray>& Map, UObject* Object);
};

UCLASS(BlueprintType)
class UFilteredActorObject : public UObject
{
	GENERATED_BODY()

public:
	// UPROPERTY(BlueprintReadWrite)
	// FFPFilteredActorArray FilteredArray;
};

UCLASS(BlueprintType)
class UValidActorList : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> AllActors;

	UFUNCTION(BlueprintCallable, Category = "ValidActorList")
	const TArray<AActor*>& GetActors() { return AllActors; }

	UFUNCTION(BlueprintCallable, Category = "ValidActorList")
	void AddActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "ValidActorList")
	void RemoveActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "ValidActorList")
	void SetActors(const TArray<AActor*>& Actors);

	UFUNCTION(BlueprintCallable, Category = "ValidActorList")
	void ClearActors();

	UFUNCTION()
	void HandleActorDestroyed(AActor* Actor);
};