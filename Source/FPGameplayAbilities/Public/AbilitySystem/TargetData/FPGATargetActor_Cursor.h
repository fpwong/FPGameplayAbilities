// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Perception/AIPerceptionTypes.h"
#include "FPGAGameplayAbilityTargetActor.h"
#include "FPGATargetActor_Cursor.generated.h"

class ACharacter;

UENUM(BlueprintType)
enum EFPGACursorTargetType
{
	FPGA_Target_Actor UMETA(DisplayName = "Actor"),
	FPGA_Target_Ground UMETA(DisplayName = "Ground"),
};

UCLASS(Blueprintable, Notplaceable, Config = Game)
class FPGAMEPLAYABILITIES_API AFPGATargetActor_Cursor : public AFPGAGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AFPGATargetActor_Cursor(const FObjectInitializer& ObjectInitializer);

private:
	ACharacter* CurrentlyHovered;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
	TEnumAsByte<EFPGACursorTargetType> TargetType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Trace)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	virtual void ConfirmTargeting() override;

	virtual void Tick(float DeltaSeconds) override;

	FHitResult GetCursorHit();

	virtual FGameplayAbilityTargetDataHandle MakeTargetDataHandleFromHitResult(FHitResult& Hit);

	virtual void DrawDebugFromHit(FHitResult& Hit);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void CancelTargeting() override;

protected:
	TWeakObjectPtr<AGameplayAbilityWorldReticle> ReticleActor;
};
