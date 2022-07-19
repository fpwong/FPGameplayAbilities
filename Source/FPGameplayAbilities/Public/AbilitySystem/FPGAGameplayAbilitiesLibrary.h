// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTargetDataFilter.h"

#include "FPGAGameplayAbilitiesLibrary.generated.h"

class UGameplayAbility;
class UAttributeSet;
class UAbilitySystemComponent;
class UGameplayEffect;
class UDataTable;


UCLASS()
	class FPGAMEPLAYABILITIES_API UFPGAGameplayAbilitiesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "AbilitySystemComponent")
	static void InitGlobalData();
	
	UFUNCTION(BlueprintCallable, Category = "FPGAAbility|Attribute")
	static void AddAttributeSet(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UAttributeSet> Attributes, const UDataTable* DataTable, FName GroupName);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayAbilitySpecHandle GiveAbility(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability);
	
	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayAbilitySpecHandle GiveAbilityAndActivateOnce(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> Ability, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static UGameplayAbility* FindAbilityFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayAbilitySpec FindAbilitySpecFromClass(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> InAbilityClass);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayAbilitySpec FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void GetActivatableAbilities(UAbilitySystemComponent* AbilitySystem, TArray<FGameplayAbilitySpec>& ActivatableAbilities);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static UGameplayAbility* GetAbilityFromSpec(FGameplayAbilitySpec Spec);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayAbilitySpecHandle GetHandleFromSpec(FGameplayAbilitySpec Spec);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void ExecuteGameplayCueForAbilitySystem(UAbilitySystemComponent* AbilitySystem, const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	/**
	* Attempts to activate the given ability, will check costs and requirements before doing so.
	* Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	* If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	*/
	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static bool TryActivateAbility(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle AbilityToActivate, bool bAllowRemoteActivation = true);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayAbilitySpecHandle TryActivateAbilityWithEvent(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData EventData);

	static UGameplayAbility* ActivateAbilityWithEvent(UAbilitySystemComponent* AbilitySystem, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData EventData, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate = nullptr);

	/** Gets the current AbilitySpecHandle- can only be called on instanced abilities. */
	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayAbilitySpecHandle GetAbilitySpecHandle(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static int32 HandleGameplayEvent(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTag EventTag, FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FPGAAbility")
	static bool IsAbilityActive(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void GetCooldownTimeRemainingAndDuration(UGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle, float& TimeRemaining, float& CooldownDuration);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void GetActiveAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void AddLooseGameplayTag(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void RemoveLooseGameplayTag(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void BlockAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void UnBlockAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static void CancelAbilitiesWithTags(UAbilitySystemComponent* AbilitySystem, UGameplayAbility* RequestingAbility, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static UAttributeSet* FindAttributeSetOfClass(UAbilitySystemComponent* AbilitySystem, const TSubclassOf<UAttributeSet> AttributeClass);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
	static FGameplayEffectContextHandle GetContextFromEffectSpec(const FGameplayEffectSpec& Spec);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|EffectContext", Meta = (DisplayName = "GetActors"))
    static TArray<AActor*> EffectContectGetActors(FGameplayEffectContextHandle EffectContext);

	// CALCULATIONS ----------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility|Calculation")
	static UAbilitySystemComponent* GetTargetAbilitySystemComponent(FGameplayEffectCustomExecutionParameters ExecutionParams);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|Calculation")
	static UAbilitySystemComponent* GetSourceAbilitySystemComponent(FGameplayEffectCustomExecutionParameters ExecutionParams);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|Calculation")
	static const FGameplayEffectSpec GetOwningSpec(FGameplayEffectCustomExecutionParameters ExecutionParams);

	UFUNCTION(BlueprintCallable, Category = "FPGAAbility")
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
	//UFUNCTION(BlueprintCallable, Category = "FPGAAbility|Calculation")
	//	static bool AttemptCalculateCapturedAttributeMagnitude(FGameplayEffectCustomExecutionParameters ExecutionParams, const FGameplayEffectAttributeCaptureDefinition InCaptureDef, float& OutMagnitude);

	//UFUNCTION(BlueprintPure, Category = "FPGAAbility|Calculation")
	//	static FGameplayEffectAttributeCaptureDefinition GetCaptureFromAttribute(TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture, FGameplayAttribute Attribute);

	//UFUNCTION(BlueprintCallable, Category = "FPGAAbility|Calculation")
	//	static void AddOutputModifier(TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture, FGameplayEffectFPGAExecutionOutput& OutExecutionOutput, const FGameplayAttribute InAttribute, TEnumAsByte<EGameplayModOp::Type> InModOp, float InMagnitude);

	//UFUNCTION(BlueprintPure, Category = "FPGAAbility|Calculation")
	//	static bool GetCapturedAttributeMagnitude(FGameplayAttribute Attribute,
	//		TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture,
	//		FGameplayEffectCustomExecutionParameters ExecutionParams,
	//		float& OutMagnitude);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|Query")
	static FGameplayTagQuery Query_MatchTag(FGameplayTag const& InTag);

	UFUNCTION(BlueprintPure, Category = "Effect|Query")
	static FGameplayEffectQuery Query_MatchAnyOwningTags(const FGameplayTagContainer& InTags);

	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	static class UGameplayEffectUIData* GetGameplayEffectUIData(FActiveGameplayEffectHandle Handle);

	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	static UAbilitySystemComponent* GetAbilitySystemFromEffectHandle(FActiveGameplayEffectHandle Handle);

	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	static class UAbilitySystemComponent* GetInstigatorAbilitySystemFromEffectSpec(const FGameplayEffectSpec& Spec);

	// UFUNCTION(BlueprintPure, Category = "FPGAAbility|Target Data")
	// static FVector GetLocationFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|Target Data")
	static bool GetLocationFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index, FVector& Location);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|Target Data")
	static AActor* GetFirstActorFromTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|Target Data")
	static FGameplayAbilityTargetDataHandle MakeTargetDataFromLocations(FVector Source, FVector Target);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility|Target Data")
	static FGameplayAbilityTargetDataHandle MakeVectorTargetData(FVector Location);

	// UFUNCTION(BlueprintPure, Category = "FPGAAbility|Target Data")
	// static FGameplayAbilityTargetData MakeTargetDataFromHitData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility")
	static bool CanActivateAbilityFromHandle(UAbilitySystemComponent* AbilitySystem, FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintPure, Category = "FPGAAbility")
	static bool CanActivateAbility(UAbilitySystemComponent* AbilitySystem, UGameplayAbility* Ability);

	/** Sets the magnitude of a SetByCaller modifier */
	UFUNCTION(BlueprintCallable, Category = "Effect")
	static void SetSetByCallerMagnitude(UPARAM(ref) FGameplayEffectSpec& Spec, FGameplayTag DataTag, float Magnitude);

	UFUNCTION(BlueprintPure, Category = "Effect")
    static FGameplayEffectSpec GetEffectSpecFromHandle(UPARAM(ref) FGameplayEffectSpecHandle& Spec);

	//////////////
	//// Tags
	//////////////
	UFUNCTION(BlueprintCallable, Category = "Tags")
	static int32 GetTagCount(UAbilitySystemComponent* AbilitySystem, FGameplayTag Tag);

	///** Create a handle for filtering target data, filling out all fields */
	//UFUNCTION(BlueprintPure, Category = "Filter")
	//static FGameplayTargetDataFilterHandle MakeFPGAFilterHandle(FFPGAGameplayTargetDataFilter Filter, AActor* FilterActor);

	/* Idea from https://github.com/DaedalicEntertainment/ue4-orders-abilities */
	UFUNCTION(BlueprintCallable)
	static void FillRelationshipTags(UPARAM(ref) FGameplayTagContainer& TagContainer, const AActor* Source, const AActor* Target);
};