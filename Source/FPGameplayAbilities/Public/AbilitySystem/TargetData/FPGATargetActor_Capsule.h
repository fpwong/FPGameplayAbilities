// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Perception/AIPerceptionTypes.h"
#include "FPGAGameplayAbilityTargetActor.h"
#include "FPGATargetActor_Capsule.generated.h"

class ACharacter;

UCLASS(Blueprintable, Notplaceable, Config = Game)
class FPGAMEPLAYABILITIES_API AFPGATargetActor_Capsule : public AFPGAGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AFPGATargetActor_Capsule(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
	float Radius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
	float HalfHeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
	float Angle;

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	virtual void Tick(float DeltaSeconds) override;

	TArray<AActor*> RunTrace();

	FVector GetTraceLocation();

	virtual void DrawDebugFromHit(const TArray<AActor*>& HitActors, float Duration);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void CancelTargeting() override;

protected:
	TWeakObjectPtr<AGameplayAbilityWorldReticle> ReticleActor;

private:
	float AngleCos;
};
