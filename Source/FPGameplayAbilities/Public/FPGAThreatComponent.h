// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPGAThreatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPGAMEPLAYABILITIES_API UFPGAThreatComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFPGAOnThreatChanged, UFPGAThreatComponent*, OldTarget, UFPGAThreatComponent*, NewTarget);

	UPROPERTY(BlueprintAssignable)
	FFPGAOnThreatChanged OnThreatChanged;

	UFPGAThreatComponent();

	UFUNCTION(BlueprintCallable)
	void AddThreat(UFPGAThreatComponent* Target, float Threat);

	float GetThreat(UFPGAThreatComponent* Target);

	UFPGAThreatComponent* GetAggroTarget();

	void UpdateAggroTarget(UFPGAThreatComponent* NewTarget);

	void SetAggroTarget(UFPGAThreatComponent* NewTarget);

	UFPGAThreatComponent* GetHighestThreat();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<TWeakObjectPtr<UFPGAThreatComponent>, float> ThreatTable;

	UPROPERTY()
	TWeakObjectPtr<UFPGAThreatComponent> AggroTarget;
};
