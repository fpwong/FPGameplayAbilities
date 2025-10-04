// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "GameplayCueNotify_HitImpact.h"
#include "UObject/Object.h"
#include "FPGameplayCue_Beam.generated.h"

class UNiagaraComponent;
class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API UFPGameplayCue_Beam : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	TObjectPtr<USoundBase> Sound;

	/** Effects to play for weapon attacks against specific surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	TObjectPtr<UNiagaraSystem> BeamNiagaraSystem;

	/** Effects to play for weapon attacks against specific surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	TObjectPtr<UNiagaraSystem> HitNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bAttachEmitter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	bool bRotateHitTowardsDir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta=(EditCondition="bAttachEmitter"))
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta=(AllowPreserveRatio))
	FVector ParticleSystemScale = FVector(1.0f);

	virtual bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

	virtual void SpawnParticle(AActor* MyTarget, const FGameplayCueParameters& Parameters);

	UFUNCTION(BlueprintNativeEvent)
	void OnParticleSpawned(UNiagaraComponent* Niagara, UParticleSystemComponent* Cascade);

	UFUNCTION(BlueprintCallable)
	static FGameplayCueParameters MakeBeamCueParams(FVector Start, FVector End);
};
