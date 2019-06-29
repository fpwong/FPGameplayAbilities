// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "FPGAAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(Blueprintable, BlueprintType)
	class FPGAMEPLAYABILITIES_API UFPGAAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFPGAAttributeSet();

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;

	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	void HandleHealthLost(AActor* SourceActor, float HealthLost, const float HealthBeforeLost);

public:

	/** Damage Attribute */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UFPGAAttributeSet, Damage)

	UFUNCTION()
	void OnRep_Damage()
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFPGAAttributeSet, Damage);
	}

	/** Health Attribute */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UFPGAAttributeSet, Health)

	UFUNCTION()
	void OnRep_Health()
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFPGAAttributeSet, Health);
	}

	/** MaxHealth Attribute */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UFPGAAttributeSet, MaxHealth)

	UFUNCTION()
	void OnRep_MaxHealth()
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFPGAAttributeSet, MaxHealth);
	}

	/** MovementSpeed Attribute */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UFPGAAttributeSet, MovementSpeed)

	UFUNCTION()
	void OnRep_MovementSpeed()
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UFPGAAttributeSet, MovementSpeed);
	}
};
