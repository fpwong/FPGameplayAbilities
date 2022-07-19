// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "Delegates/IDelegateInstance.h"
#include "FPGAGameplayAbility_Old.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayAbilityCooldown, UGameplayAbility*, Ability);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayAbilityEvent, UGameplayAbility*, Ability);

UCLASS()
class FPGAMEPLAYABILITIES_API UFPGAGameplayAbility_Old : public UGameplayAbility
{
	GENERATED_BODY()

public:
	FDelegateHandle MyDelHandle;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayAbilityEvent OnAbilityCooldown;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayAbilityEvent OnAbilityActivated;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayAbilityEvent OnAbilityCommitted;

	UPROPERTY(BlueprintAssignable)
	FOnGameplayAbilityEvent OnAbilityEnded;

	UFUNCTION()
	void OnCooldownFinished(const FGameplayEffectRemovalInfo& RemovalInfo);


	UFUNCTION()
	void OnGameplayEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	/** Does the commit atomically (consume resources, do cooldowns, etc) */
	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	virtual bool CommitAbilityCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const bool ForceCooldown);

	void FPGAApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	UFUNCTION(BlueprintCallable, Category="Ability|Cooldown")
	virtual bool HasCooldown();

	/** Checks cooldown. returns true if we can be used again. False if not */
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** Returns the time in seconds remaining on the currently active cooldown. */
	virtual float GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const override;

	/** Returns the time in seconds remaining on the currently active cooldown and the original duration for this cooldown. */
	virtual void GetCooldownTimeRemainingAndDuration(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const override;

	/** Checks cost. returns true if we can pay for the ability. False if not */
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;


	UFUNCTION(BlueprintImplementableEvent, Category="Ability")
	void AdjustCostSpec(UPARAM(ref) FGameplayEffectSpec& CostSpec) const;

	UFUNCTION(BlueprintImplementableEvent, Category="Ability")
	void AdjustCooldownSpec(UPARAM(ref) FGameplayEffectSpec& CooldownSpec) const;

	void SetCostEffectClass(TSubclassOf<UGameplayEffect> CostEffectClass) { CostGameplayEffectClass = CostEffectClass; }
	void SetCooldownEffectClass(TSubclassOf<UGameplayEffect> CooldownEffectClass) { CooldownGameplayEffectClass = CooldownEffectClass; }

	UFUNCTION(BlueprintCallable, Category="Ability")
	void BlockAndCancelTags();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FActiveGameplayEffectHandle CooldownEffectHandle;
};
