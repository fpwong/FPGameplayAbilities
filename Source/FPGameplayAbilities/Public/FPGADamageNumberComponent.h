// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "FPGADamageNumberComponent.generated.h"

class UFPGAUWDamageNumber;

USTRUCT()
struct FDamageNumberInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> DamagedActor;

	UPROPERTY()
	float DamageAmount = 0.0f;

	UPROPERTY()
	FGameplayTagContainer DamageTags;
};

struct FActorDamageNumberInfo
{
	TQueue<FDamageNumberInfo> PendingDamageNumbers;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPGAMEPLAYABILITIES_API UFPGADamageNumberComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGAUWDamageNumber> DamageNumberClass;

	// Sets default values for this component's properties
	UFPGADamageNumberComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Multicast_Damage(AActor* DamagedActor, float DamageAmount, const FGameplayTagContainer& DamageTags);

	void TryPlayDamageNumber();

	void PlayNextDamageNumber();

	FActorDamageNumberInfo ActorDamageInfo;
	// TMap<TWeakObjectPtr<AActor>, FActorDamageNumberInfo> PendingDamageNumbers;

	FTimerHandle TimerHandle;

	bool bWaitingForDamageNumber = false;

	float LastPlayed = -1.0f;

	float Delay = 0.1f;
};
