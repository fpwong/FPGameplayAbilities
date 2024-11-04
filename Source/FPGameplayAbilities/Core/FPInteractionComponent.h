// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGAOverlapManager.h"
#include "UObject/Object.h"
#include "FPInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFPOnFocusedActorChanged, AActor*, Old, AActor*, New);

UINTERFACE()
class UFPInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class IFPInteractInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnGainFocus();

	UFUNCTION(BlueprintNativeEvent)
	void OnLoseFocus();

	UFUNCTION(BlueprintNativeEvent)
	void OnInteract(AActor* Source);
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class FPGAMEPLAYABILITIES_API UFPInteractionComponent : public UFPGAOverlapManager
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<AActor> FocusedActor;

	UPROPERTY(BlueprintAssignable)
	FFPOnFocusedActorChanged OnFocusedActorChanged;

	UFUNCTION(BlueprintCallable)
	bool TryInteractWithFocus();

	UFUNCTION(BlueprintCallable)
	AActor* GetFocusedActor() const { return FocusedActor.Get(); };

	UFPInteractionComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void BeginOverlap(AActor* Actor);

	UFUNCTION()
	void EndOverlap(AActor* Actor);

	void ChangeFocusedActor(AActor* Old, AActor* New);

	float GetDistToActor(AActor* Actor);
};
