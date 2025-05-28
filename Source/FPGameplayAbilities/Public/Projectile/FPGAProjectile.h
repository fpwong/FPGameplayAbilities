// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"
#include "FPGAProjectile.generated.h"

class UProjectileMovementComponent;

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGAProjectileEffectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayEffectSpecHandle OnHitGameplayEffect;
};

UCLASS()
class FPGAMEPLAYABILITIES_API AFPGAProjectile : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static AFPGAProjectile* SpawnProjectile(
		const UObject* WorldContextObject,
		TSubclassOf<AFPGAProjectile> ProjectileClass,
		AActor* InOwner,
		const FTransform& Transform,
		const FGameplayAbilityTargetDataHandle& InTargetData,
		const FFPGAProjectileEffectData& InProjectileEffectData);

	FGameplayAbilityTargetDataHandle TargetData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> VisualRootComponent;

	UPROPERTY()
	FFPGAProjectileEffectData ProjectileEffectData;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag OnHitGameplayCueTag;

	TWeakObjectPtr<USceneComponent> TargetSceneComponent; 

	// Sets default values for this actor's properties
	AFPGAProjectile();

	UFUNCTION(BlueprintCallable)
	void InitProjectile(const FGameplayAbilityTargetDataHandle& InTargetData, const FFPGAProjectileEffectData& InProjectileEffectData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void Destroyed() override;

	virtual void PostNetReceiveLocationAndRotation() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool ApplyEffect(AActor* TargetActor);

	UFUNCTION()
	void OnTargetDestroyed(AActor* Actor);

	UFUNCTION()
	void HandleOnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void HandleOnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	void ExecuteOnHitGameplayCue();
};
