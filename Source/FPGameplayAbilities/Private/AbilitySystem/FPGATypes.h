#pragma once
#include "GameplayEffectTypes.h"

#include "FPGATypes.generated.h"

class UGameplayAbility;

DECLARE_MULTICAST_DELEGATE_OneParam(FFPOnAbilityReady, UGameplayAbility*);

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
