// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGATargetData.h"
#include "FPGATargetingRequest.h"
#include "FPGATargetingSystemSettingsData.h"
#include "FPGATargetingSubsystem.generated.h"

UCLASS(DisplayName = "FP Targeting Subsystem")
class FPGAMEPLAYABILITIES_API UFPGATargetingSubsystem
	: public ULocalPlayerSubsystem
	, public FTickableGameObject
{
	GENERATED_BODY()

public:
	static UFPGATargetingSubsystem* Get(const UWorld* World);
	static UFPGATargetingSubsystem* Get(const ULocalPlayer* LocalPlayer);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	DECLARE_MULTICAST_DELEGATE_OneParam(FFPTargetingEnd, FGameplayAbilityTargetDataHandle);

	FSimpleDelegate OnTargetingStart;
	FFPTargetingEnd OnTargetingEnd;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFPGAOnActorChanged, AActor*, OldActor, AActor*, NewActor);

	UPROPERTY(BlueprintAssignable)
	FFPGAOnActorChanged OnHoveredActorChanged;

	UPROPERTY(BlueprintAssignable)
	FFPGAOnActorChanged OnFocusedActorChanged;

	TWeakPtr<FFPGATargetingRequest> CurrentRequest;

	UPROPERTY()
	UFPGATargetingSystemSettingsData* SettingsData;

	void StartTargetingRequest(TSharedPtr<FFPGATargetingRequest> Request);

	bool ConfirmTargeting(const TArray<FHitResult>& HitResults);

	void CancelTargetingRequest();

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual ETickableTickType GetTickableTickType() const override { return (HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Conditional); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UFPGATargetingSubsystem, STATGROUP_Tickables); }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	// ~FTickableGameObject interface

	UFUNCTION(BlueprintCallable)
	bool GetHitResultsUnderCursor(TArray<FHitResult>& OutHitResults, float HitResultTraceDistance = 10000.0f);

	UFUNCTION()
	void UpdatePlayerFocus(const TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable)
	virtual void SetFocus(AActor* NewFocus);

	UFUNCTION()
	void FocusHoveredActor();

	UFUNCTION()
	void UpdateMouseCursor();

	UFUNCTION()
	AActor* GetFocusedActor() const { return FocusedActor.Get(); }

	UFUNCTION()
	AActor* GetHoveredActor() const { return HoveredActor.Get(); }

	UFUNCTION()
	APlayerController* GetPlayerController();

	UFUNCTION()
	APawn* GetControlledPawn();

protected:
	void HandleHoveredActorChanged(AActor* OldActor, AActor* NewActor);

	/** Actor currently focused by the player. */
	TWeakObjectPtr<AActor> FocusedActor;

	/** Actor currently hovered by the player. */
	TWeakObjectPtr<AActor> HoveredActor;

	void SetMouseCursor(EMouseCursor::Type Cursor);

	EMouseCursor::Type HoveredCursorState;
};
