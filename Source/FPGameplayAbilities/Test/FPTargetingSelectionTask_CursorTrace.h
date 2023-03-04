// // Copyright Epic Games, Inc. All Rights Reserved.
// #pragma once
//
// #include "CollisionShape.h"
// #include "ScalableFloat.h"
// #include "Engine/CollisionProfile.h"
// #include "GameplayTargetingSystem/Tasks/TargetingTask.h"
// #include "GameplayTargetingSystem/Types/TargetingSystemTypes.h"
// #include "UObject/Object.h"
// #include "FPTargetingSelectionTask_CursorTrace.generated.h"
//
// class UTargetingSubsystem;
// struct FCollisionQueryParams;
// struct FTargetingDebugInfo;
// struct FTargetingDefaultResultData;
// struct FTargetingRequestHandle;
// struct FTraceDatum;
// struct FTraceHandle;
//
//
// /**
// *	@enum ETargetingAOEShape	
// */
// UENUM()
// enum class EFPTargetingTraceType : uint8
// {
// 	Line,
// 	Sweep,
// };
//
// /**
// *	@class UFPTargetingSelectionTask_CursorTrace
// *
// *	Selection task that can perform a synchronous or asynchronous trace (line/sweep)
// *	to find all targets up to the first blocking hit (or its end point).
// */
// UCLASS(Blueprintable)
// class FPGAMEPLAYABILITIES_API UFPTargetingSelectionTask_CursorTrace : public UTargetingTask
// {
// 	GENERATED_BODY()
//
// public:
// 	UFPTargetingSelectionTask_CursorTrace(const FObjectInitializer& ObjectInitializer);
//
// 	virtual void Init(const FTargetingRequestHandle& TargetingHandle) const override;
//
// 	/** Evaluation function called by derived classes to process the targeting request */
// 	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;
//
// 	virtual void CancelAsync() const override;
//
//
// protected:
// 	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
// 	FVector GetSourceLocation(const FTargetingRequestHandle& TargetingHandle) const;
//
// 	/** Native Event to get the direction for the Trace */
// 	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
// 	FVector GetTraceDirection(const FTargetingRequestHandle& TargetingHandle) const;
//
// 	/** Native Event to get the length for the Trace */
// 	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
// 	float GetTraceLength(const FTargetingRequestHandle& TargetingHandle) const;
//
// 	/** Native Event to get the swept trace radius (only called if bSweptTrace is true) */
// 	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
// 	float GetSweptTraceRadius(const FTargetingRequestHandle& TargetingHandle) const;
//
// 	/** Native Event to get additional actors the Trace should ignore */
// 	UFUNCTION(BlueprintNativeEvent, Category = "Target Trace Selection")
// 	void GetAdditionalActorsToIgnore(const FTargetingRequestHandle& TargetingHandle, TArray<AActor*>& OutAdditionalActorsToIgnore) const;
//
// private:
// 	/** Method to process the trace task immediately */
// 	void ExecuteImmediateTrace(const FTargetingRequestHandle& TargetingHandle) const;
//
// 	/** Method to process the trace task asynchronously */
// 	void ExecuteAsyncTrace(const FTargetingRequestHandle& TargetingHandle) const;
//
// 	/** Callback for an async trace */
// 	void HandleAsyncTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum, FTargetingRequestHandle TargetingHandle) const;
//
// 	/** Method to take the hit results and store them in the targeting result data */
// 	void ProcessHitResults(const FTargetingRequestHandle& TargetingHandle, const TArray<FHitResult>& Hits) const;
//
// 	/** Setup CollisionQueryParams for the trace */
// 	void InitCollisionParams(const FTargetingRequestHandle& TargetingHandle, FCollisionQueryParams& OutParams) const;
//
// protected:
// 	/** The trace type to use */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Collision Data")
// 	EFPTargetingTraceType TraceType = EFPTargetingTraceType::Line;
//
// 	/** The trace channel to use */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Collision Data")
// 	TEnumAsByte<ETraceTypeQuery> TraceChannel;
//
// 	/** The collision profile name to use instead of trace channel (does not work for async traces) */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Collision Data")
// 	FCollisionProfileName CollisionProfileName;
//
// 	/** The default swept trace radius used by GetSweptTraceRadius when the trace type is set to Sweep */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Swept Data")
// 	FScalableFloat DefaultSweptTraceRadius = 10.0f;
//
// 	/** The default trace length to use if GetTraceLength is not overridden by a child */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
// 	FScalableFloat DefaultTraceLength = 10000.0f;
//
// 	/** Indicates the trace should perform a complex trace */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
// 	uint8 bComplexTrace : 1;
//
// 	/** Indicates the trace should ignore the source actor */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
// 	uint8 bIgnoreSourceActor : 1;
//
// 	/** Indicates the trace should ignore the instigator actor */
// 	UPROPERTY(EditAnywhere, Category = "Target Trace Selection | Trace Data")
// 	uint8 bIgnoreInstigatorActor : 1;
//
// protected:
// #if WITH_EDITOR
// 	virtual bool CanEditChange(const FProperty* InProperty) const override;
// #endif
//
// 	/** Debug Helper Methods */
// #if ENABLE_DRAW_DEBUG
// private:
// 	virtual void DrawDebug(UTargetingSubsystem* TargetingSubsystem, FTargetingDebugInfo& Info, const FTargetingRequestHandle& TargetingHandle, float XOffset, float YOffset, int32 MinTextRowsToAdvance) const override;
// 	void BuildTraceResultsDebugString(const FTargetingRequestHandle& TargetingHandle, const TArray<FTargetingDefaultResultData>& TargetResults) const;
// 	void ResetTraceResultsDebugString(const FTargetingRequestHandle& TargetingHandle) const;
// #endif // ENABLE_DRAW_DEBUG
// 	/** ~Debug Helper Methods */
// };
//
