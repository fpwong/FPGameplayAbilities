// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"
#include "FPGAProjectile.generated.h"

class UProjectileMovementComponent;
UCLASS()
class FPGAMEPLAYABILITIES_API AFPGAProjectile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	FGameplayAbilityTargetDataHandle TargetData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileMovementComponent;

	TWeakObjectPtr<USceneComponent> TargetSceneComponent; 

	// Sets default values for this actor's properties
	AFPGAProjectile();

	UFUNCTION(BlueprintCallable)
	void InitProjectile(const FGameplayAbilityTargetDataHandle& InTargetData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void Destroyed() override;

	virtual void PostNetReceiveLocationAndRotation() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnTargetDestroyed(AActor* Actor);
};
