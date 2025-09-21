// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPGameplayAbilities/Targeting/FPTargetFilterObserver.h"
#include "FPGAOverlapManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFPGAActorOverlapEvent, AActor*, Actor);

USTRUCT()
struct FFPGAOverlapInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> Actor;

	UPROPERTY()
	FFPTargetFilterObserver FilterObserver;

	FFPGAOverlapInstance() = default;
	FFPGAOverlapInstance(const FFPTargetFilter& TargetFilter, AActor* Source, AActor* Target);
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
	bool bBindToOwner = true;

	UFUNCTION(BlueprintCallable)
	void SetSourceActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void BindToPrimitiveComponent(UPrimitiveComponent* Component);

	UFUNCTION(BlueprintCallable)
	void BindToActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void SetFilterTaskSet(const FFPTargetFilter& InFilterTaskSet);

protected:
	UPROPERTY()
	TWeakObjectPtr<UPrimitiveComponent> CollidingComponent;

	// UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, TSharedRef<FFPGAOverlapInstance>> AllOverlappingActors;

	TArray<TObjectPtr<AActor>> ValidOverlappingActors;

	UPROPERTY(EditDefaultsOnly)
	FFPTargetFilter TargetFilter;

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
