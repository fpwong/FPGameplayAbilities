﻿#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "FPGATypes.generated.h"

class UGameplayAbility;

DECLARE_MULTICAST_DELEGATE_OneParam(FFPOnAbilityReady, UGameplayAbility*);
DECLARE_MULTICAST_DELEGATE_OneParam(FPPlayerStateDelegate, APlayerState*);

UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPGameplayInterface : public UInterface
{
	GENERATED_BODY()
};

class FPGAMEPLAYABILITIES_API IFPGameplayInterface
{
	GENERATED_BODY()

public:
	virtual bool CanMove(UAbilitySystemComponent* ASC) { return true; }
	virtual bool CanRotate(UAbilitySystemComponent* ASC) { return true; }
	virtual float GetAttackRange(UAbilitySystemComponent* ASC) { return 300.0f; }
};

UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class FPGAMEPLAYABILITIES_API IFPAbilityInterface
{
	GENERATED_BODY()

public:
	virtual FGameplayTagContainer GetAbilityTypeTags() const { return FGameplayTagContainer(); }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="FPCommon")
	float GetAbilityRange(UAbilitySystemComponent* ASC);
};

UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPGameplayEffectInterface : public UInterface
{
	GENERATED_BODY()
};


class FPGAMEPLAYABILITIES_API IFPGameplayEffectInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="FPCommon")
	FGameplayTag GetPeriodValueTag(UAbilitySystemComponent* ASC);
};


UENUM(BlueprintType)
enum class EFPGACancelLevel : uint8
{
	None,
	OnlyBackswing,
	All
};

USTRUCT(BlueprintType)
struct FPGAMEPLAYABILITIES_API FFPGAAbilityReadyBinding
{
	GENERATED_BODY()

	FFPOnAbilityReady OnAbilityReady;

	TWeakObjectPtr<UGameplayAbility> BoundAbility;
	TWeakObjectPtr<UAbilitySystemComponent> BoundAbilitySystem;

	bool IsBound() { return BoundAbility.IsValid(); }

	void Clear();

	void BindToGameplayAbility(UGameplayAbility* Ability);

	void OnCooldownEffectRemoved(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo);

	void CheckAbilityReady();

protected:
	void OnAbilityEnded(UGameplayAbility* GameplayAbility);

	TSet<FDelegateHandle> BoundCooldownDelegates;
	TSet<FActiveGameplayEffectHandle> BoundHandles;
};

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FPGAMEPLAYABILITIES_API FFPSetByCallerMagnitude
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag DataTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Magnitude = 0;
};

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FPGAMEPLAYABILITIES_API FFPTableGameplayEffectParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UDataTable> DataTable;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag EffectPeriodTag;
};

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FPGAMEPLAYABILITIES_API FFPGameplayEffectParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ShowOnlyInnerProperties))
	TArray<FFPSetByCallerMagnitude> SetByCallerParameters;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ShowOnlyInnerProperties))
	TArray<FFPTableGameplayEffectParameters> TableParameters;

	void ApplyToGameplayEffectSpec(UAbilitySystemComponent* ASC, TSharedPtr<FGameplayEffectSpec> Spec) const;
};

UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPPlayerStateInterface : public UInterface
{
	GENERATED_BODY()
};

class FPGAMEPLAYABILITIES_API IFPPlayerStateInterface
{
	GENERATED_BODY()

public:
	virtual FSimpleMulticastDelegate& GetPlayerNameChangeDelegate() = 0;
};

 

UINTERFACE()
class FPGAMEPLAYABILITIES_API UFPGameStateInterface : public UInterface
{
	GENERATED_BODY()
};

class FPGAMEPLAYABILITIES_API IFPGameStateInterface
{
	GENERATED_BODY()

public:
	virtual FPPlayerStateDelegate& GetPlayerLoginDelegate() = 0;
	virtual FPPlayerStateDelegate& GetPlayerLogoutDelegate() = 0;
};

USTRUCT(BlueprintType)
struct FFPTaggedAttribute
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;
};
