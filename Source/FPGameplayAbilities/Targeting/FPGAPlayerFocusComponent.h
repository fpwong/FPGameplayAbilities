// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGATargetFilter.h"
#include "Components/ActorComponent.h"
#include "FPGAPlayerFocusComponent.generated.h"

// This class does not need to be modified.
UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPGAPlayerFocusInterface : public UInterface
{
	GENERATED_BODY()
};

class FPGAMEPLAYABILITIES_API IFPGAPlayerFocusInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Player Focus")
	void OnBeginHovered();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Player Focus")
	void OnEndHovered();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Player Focus")
	void OnBeginFocus();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Player Focus")
	void OnEndFocus();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFPOnHoveredActorChanged, AActor*, OldActor, AActor*, NewActor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPGAMEPLAYABILITIES_API UFPGAPlayerFocusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FFPOnHoveredActorChanged OnHoveredActorChanged;

	UFPGAPlayerFocusComponent();

	virtual void UpdateStateFromHitResults(const TArray<FHitResult>& HitResults);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Actor currently focused by the player. */
	TWeakObjectPtr<AActor> FocusedTarget;

	/** Actor currently hovered by the player. */
	TWeakObjectPtr<AActor> HoveredActor;

	UPROPERTY(EditDefaultsOnly)
	FFPGATargetFilter TargetFilter;
};
