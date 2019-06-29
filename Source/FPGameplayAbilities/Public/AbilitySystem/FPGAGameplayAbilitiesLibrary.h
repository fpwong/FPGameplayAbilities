// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "UObject/ScriptInterface.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayCueInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetDataFilter.h"
#include "GameplayEffectExecutionCalculation.h"

#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySet.h"
#include "GameplayEffectAggregator.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectUIData.h"
#include "Perception/AISense.h"

#include "FPGAGameplayAbilitiesLibrary.generated.h"

struct FFPGAGameplayTargetDataFilter;
class UAbilitySystemComponent;
class UGameplayEffect;
class UDataTable;


UCLASS()
	class FPGAMEPLAYABILITIES_API UFPGAGameplayAbilitiesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Attribute")
	static void AddAttributeSet(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UAttributeSet> Attributes, const UDataTable* DataTable, FName GroupName);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static FGameplayAbilitySpecHandle GiveAbility(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UGameplayAbility* FindAbilityFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static FGameplayAbilitySpec FindAbilitySpecFromClass(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> InAbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static FGameplayAbilitySpec FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void GetActivatableAbilities(UAbilitySystemComponent* AbilitySystem, TArray<FGameplayAbilitySpec>& ActivatableAbilities);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UGameplayAbility* GetAbilityFromSpec(FGameplayAbilitySpec Spec);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static FGameplayAbilitySpecHandle GetHandleFromSpec(FGameplayAbilitySpec Spec);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void ExecuteGameplayCueForAbilitySystem(UAbilitySystemComponent* AbilitySystem, const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	/**
	* Attempts to activate the given ability, will check costs and requirements before doing so.
	* Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	* If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability")
	static bool TryActivateAbility(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle AbilityToActivate, bool bAllowRemoteActivation = true);

	/** Gets the current AbilitySpecHandle- can only be called on instanced abilities. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	static FGameplayAbilitySpecHandle GetAbilitySpecHandle(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static int32 HandleGameplayEvent(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
	static bool IsAbilityActive(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void GetCooldownTimeRemainingAndDuration(UGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle, float& TimeRemaining, float& CooldownDuration);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void GetActiveAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void AddLooseGameplayTag(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void RemoveLooseGameplayTag(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void BlockAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void UnBlockAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void CancelAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, UGameplayAbility* RequestingAbility, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static UAttributeSet* FindAttributeSetOfClass(UAbilitySystemComponent* AbilitySystem, const TSubclassOf<UAttributeSet> AttributeClass);

	// CALCULATIONS ----------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Ability|Calculation")
	static UAbilitySystemComponent* GetTargetAbilitySystemComponent(FGameplayEffectCustomExecutionParameters ExecutionParams);

	UFUNCTION(BlueprintPure, Category = "Ability|Calculation")
	static UAbilitySystemComponent* GetSourceAbilitySystemComponent(FGameplayEffectCustomExecutionParameters ExecutionParams);

	UFUNCTION(BlueprintPure, Category = "Ability|Calculation")
	static const FGameplayEffectSpec GetOwningSpec(FGameplayEffectCustomExecutionParameters ExecutionParams);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	static void InitAbilityActorInfo(UAbilitySystemComponent* AbilitySystemComponent, AActor* InOwnerActor, AActor* InAvatarActor);

	///**
	//* Attempts to calculate the magnitude of a captured attribute given the specified parameters. Can fail if the gameplay spec doesn't have
	//* a valid capture for the attribute.
	//*
	//* @param InCaptureDef	Attribute definition to attempt to calculate the magnitude of
	//* @param InEvalParams	Parameters to evaluate the attribute under
	//* @param OutMagnitude	[OUT] Computed magnitude
	//*
	//* @return True if the magnitude was successfully calculated, false if it was not
	//*/
	//UFUNCTION(BlueprintCallable, Category = "Ability|Calculation")
	//	static bool AttemptCalculateCapturedAttributeMagnitude(FGameplayEffectCustomExecutionParameters ExecutionParams, const FGameplayEffectAttributeCaptureDefinition InCaptureDef, float& OutMagnitude);

	//UFUNCTION(BlueprintPure, Category = "Ability|Calculation")
	//	static FGameplayEffectAttributeCaptureDefinition GetCaptureFromAttribute(TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture, FGameplayAttribute Attribute);

	//UFUNCTION(BlueprintCallable, Category = "Ability|Calculation")
	//	static void AddOutputModifier(TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture, FGameplayEffectFPGAExecutionOutput& OutExecutionOutput, const FGameplayAttribute InAttribute, TEnumAsByte<EGameplayModOp::Type> InModOp, float InMagnitude);

	//UFUNCTION(BlueprintPure, Category = "Ability|Calculation")
	//	static bool GetCapturedAttributeMagnitude(FGameplayAttribute Attribute,
	//		TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture,
	//		FGameplayEffectCustomExecutionParameters ExecutionParams,
	//		float& OutMagnitude);

	UFUNCTION(BlueprintPure, Category = "Ability|Query")
	static FGameplayTagQuery Query_MatchTag(FGameplayTag const& InTag);

	UFUNCTION(BlueprintPure, Category = "Effect|Query")
	static FGameplayEffectQuery Query_MatchAnyOwningTags(const FGameplayTagContainer& InTags);

	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	static class UGameplayEffectUIData* GetGameplayEffectUIData(FActiveGameplayEffectHandle Handle);

	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	static UAbilitySystemComponent* GetAbilitySystemFromEffectHandle(FActiveGameplayEffectHandle Handle);

	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	static class UAbilitySystemComponent* GetInstigatorAbilitySystemFromEffectSpec(const FGameplayEffectSpec& Spec);

	UFUNCTION(BlueprintPure, Category = "Ability|TargetData")
	static FVector GetLocationFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	UFUNCTION(BlueprintPure, Category = "Ability")
	static bool CanActivateAbilityFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintPure, Category = "Ability")
	static bool CanActivateAbility(UAbilitySystemComponent* AbilitySystem, UGameplayAbility* Ability);

	/** Sets the magnitude of a SetByCaller modifier */
	UFUNCTION(BlueprintCallable, Category = "Effect")
	static void SetSetByCallerMagnitude(UPARAM(ref) FGameplayEffectSpec& Spec, FGameplayTag DataTag, float Magnitude);


	//////////////
	//// Tags
	//////////////
	UFUNCTION(BlueprintCallable, Category = "Tags")
	static int32 GetTagCount(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag);

	///** Create a handle for filtering target data, filling out all fields */
	//UFUNCTION(BlueprintPure, Category = "Filter")
	//static FGameplayTargetDataFilterHandle MakeFPGAFilterHandle(FFPGAGameplayTargetDataFilter Filter, AActor* FilterActor);
};