// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "GameplayCueNotify_HitImpact.h"
#include "UObject/Object.h"
#include "FPGameplayCue.generated.h"

class UNiagaraComponent;
class UParticleSystemComponent;

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGameplayCue : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<USoundBase> Sound;

	/** Effects to play for weapon attacks against specific surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	/** Effects to play for weapon attacks against specific surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<UParticleSystem> CascadeSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	bool bAttachEmitter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue, meta=(EditCondition="bAttachEmitter"))
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	FVector ParticleSystemScale = FVector(1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	bool bUseLocationParam = false;

	virtual bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

	virtual void SpawnParticle(AActor* MyTarget, const FGameplayCueParameters& Parameters);

	UFUNCTION(BlueprintNativeEvent)
	void OnParticleSpawned(UNiagaraComponent* Niagara, UParticleSystemComponent* Cascade);

	UFUNCTION(BlueprintCallable)
	static FGameplayCueParameters MakeLocationGameplayCueParams(FVector Location, FRotator Rotation);
};
