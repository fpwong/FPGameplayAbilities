// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "AttributeSet.h"
#include "GenericTeamAgentInterface.h"
#include "FPGACharacter.generated.h"

UCLASS()
class FPGAMEPLAYABILITIES_API AFPGACharacter
	: public ACharacter
	, public IAbilitySystemInterface
	, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	/** Our ability system */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UFPGAAbilitySystemComponent* AbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
	TArray<TSubclassOf<class UGameplayAbility>> InitialAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
	TArray<TSubclassOf<class UGameplayEffect>> InitialEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
	TSubclassOf<class UAttributeSet> AttributeSetClass;

	UPROPERTY(BlueprintReadWrite, Category = Abilities)
	const UAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
	const class UDataTable* AttributeSetDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetMovementLocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefaultMovementDistance;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMovementDistanceToTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Meta = (ExposeOnSpawn="true"))
	uint8 TeamId;

	AFPGACharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Restart() override;

	FGenericTeamId GetGenericTeamId() const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UFUNCTION()
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& Effect);

	UFUNCTION()
	void OnAbilityCommitted(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable)
	void TestPrintGE();

	UFUNCTION()
	void OnGEAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;

	//void OnTurnSpeedChanged(const FOnAttributeChangeData& ChangeData);
};
