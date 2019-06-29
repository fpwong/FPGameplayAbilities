// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "FPGAPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FPGAMEPLAYABILITIES_API AFPGAPlayerState
	: public APlayerState
	, public IAbilitySystemInterface
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

	AFPGAPlayerState(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
