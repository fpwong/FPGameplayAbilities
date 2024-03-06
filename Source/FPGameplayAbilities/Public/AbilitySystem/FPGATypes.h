#pragma once
#include "GameplayEffectTypes.h"

#include "FPGATypes.generated.h"

class UGameplayAbility;

DECLARE_MULTICAST_DELEGATE_OneParam(FFPOnAbilityReady, UGameplayAbility*);

UINTERFACE()
class UFPAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class IFPAbilityInterface
{
	GENERATED_BODY()

public:
	virtual FGameplayTagContainer GetAbilityTypeTags() { return FGameplayTagContainer(); }
};

UINTERFACE()
class UFPGameplayEffectInterface : public UInterface
{
	GENERATED_BODY()
};


class IFPGameplayEffectInterface
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
	UDataTable* DataTable;

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