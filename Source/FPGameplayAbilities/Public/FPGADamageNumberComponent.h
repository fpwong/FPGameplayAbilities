// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "FPGADamageNumberComponent.generated.h"

class UFPGAUWDamageNumber;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPGAMEPLAYABILITIES_API UFPGADamageNumberComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGAUWDamageNumber> DamageNumberClass;

	// Sets default values for this component's properties
	UFPGADamageNumberComponent();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multicast_Damage(AActor* DamagedActor, float DamageAmount, const FGameplayTagContainer& DamageTags);
};
