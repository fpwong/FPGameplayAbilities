// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilterTaskSetObserver.h"
#include "FPGAOverlapManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPGAActorOverlapEvent, AActor*, Actor);

USTRUCT()
struct FFPGAOverlapInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY()
	FFPTargetFilterTaskSetObserver OngoingFilterTaskset;

	FFPGAOverlapInstance() = default;
	FFPGAOverlapInstance(const FFPTargetFilterTaskSet& Taskset, AActor* Source, AActor* Target);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPGAMEPLAYABILITIES_API UFPGAOverlapManager : public UActorComponent
{
	GENERATED_BODY()

public:

	UFPGAOverlapManager(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetOverlappingActors() const { return ValidOverlappingActors; }

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetAllOverlappingActors() const;

	UPROPERTY(BlueprintAssignable)
	FFPGAActorOverlapEvent OnBeginOverlap;

	UPROPERTY(BlueprintAssignable)
	FFPGAActorOverlapEvent OnEndOverlap;

	UPROPERTY(EditDefaultsOnly)
	bool bBindToOwner = false;

	UFUNCTION(BlueprintCallable)
	void SetSourceActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void BindToPrimitiveComponent(UPrimitiveComponent* Component);

	UFUNCTION(BlueprintCallable)
	void BindToActor(AActor* Actor);

protected:
	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> CollidingComponent;

	// UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, TSharedRef<FFPGAOverlapInstance>> AllOverlappingActors;

	TArray<TObjectPtr<AActor>> ValidOverlappingActors;

	UPROPERTY(EditDefaultsOnly)
	FFPTargetFilterTaskSet FilterTaskset;

	TWeakObjectPtr<AActor> SourceActorPtr;

	UFUNCTION()
	void HandleBeginOverlap(AActor* OtherActor);

	UFUNCTION()
	void HandleEndOverlap(AActor* OtherActor);

	UFUNCTION()
	void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnPrimitiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPrimitiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnFilterResultChanged(AActor* Target, bool bResult);
};
