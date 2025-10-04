// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayCueNotify_Actor.h"
#include "NiagaraSystem.h"

#include "FPGameplayCue_Status.generated.h"

class UNiagaraComponent;
class UParticleSystemComponent;

/*
 * Simple gameplay cue that holds 1 niagara or cascade system and attaches to target while active
 */
UCLASS()
class FPGAMEPLAYABILITIES_API AFPGameplayCue_Status : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	/** Effects to play for weapon attacks against specific surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	/** Effects to play for weapon attacks against specific surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	TObjectPtr<UParticleSystem> CascadeSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta=(AllowPreserveRatio))
	FVector ParticleSystemScale = FVector(1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	float RelativeHeightOffset = 0.0f;

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnParticleSpawned(UNiagaraComponent* Niagara, UParticleSystemComponent* Cascade);

	TWeakObjectPtr<UNiagaraComponent> ActiveNiagaraComponent;
	TWeakObjectPtr<UParticleSystemComponent> ActiveCascadeSystem;
	TWeakObjectPtr<UAudioComponent> ActiveAudioComponent;
};
