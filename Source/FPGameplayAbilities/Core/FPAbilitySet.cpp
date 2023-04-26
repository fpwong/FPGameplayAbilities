// Fill out your copyright notice in the Description page of Project Settings.

#include "FPAbilitySet.h"

#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAbilitySet)

namespace FPAbilitySetHandle_Impl
{
	static int32 LastHandleId = 0;
	static int32 GetNextQueuedHandleIdForUse() { return ++LastHandleId; }
}

bool FFPAbilitySetHandle::IsValid()
{
	return HandleId > 0;
}

void FFPAbilitySetHandle::Reset()
{
	AbilitySystemComponent.Reset();
	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	HandleId = -1;
}

void FFPAbilitySetHandle::RemoveAbilitySet()
{
	if (!IsValid())
	{
		return;
	}

	if (!AbilitySystemComponent->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystemComponent->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
		}
	}

	// for (auto Set : AbilitySetHandle.GrantedAttributeSets)
	// {
	// 	if (Set.IsValid())
	// 	{
	// 		AbilitySetHandle.AbilitySystemComponent->RemoveSpawnedAttribute(Set.Get());
	// 	}
	// }

	Reset();
}

FFPAbilitySetHandle FFPAbilitySet::GiveAbilitySetTo(UAbilitySystemComponent* ASC, UObject* OverrideSourceObject) const
{
	return GiveAbilityWithParameters(ASC, FFPAbilitySetParameters(), OverrideSourceObject);
}

FFPAbilitySetHandle FFPAbilitySet::GiveAbilityWithParameters(UAbilitySystemComponent* ASC, const FFPAbilitySetParameters& Parameters, UObject* OverrideSourceObject) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return FFPAbilitySetHandle();
	}

	FFPAbilitySetHandle OutHandle;
	OutHandle.HandleId = FPAbilitySetHandle_Impl::GetNextQueuedHandleIdForUse();
	OutHandle.AbilitySystemComponent = ASC;

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FFPGrantedAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set is not valid."), AbilityIndex);
			continue;
		}

		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = OverrideSourceObject;
		// AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		OutHandle.AbilitySpecHandles.Add(AbilitySpecHandle);
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FFPGrantedGameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set is not valid"), EffectIndex);
			continue;
		}

		// const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();

		FGameplayEffectSpecHandle GESpec = ASC->MakeOutgoingSpec(EffectToGrant.GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		for (const FFPSetByCallerMagnitude& Elem : Parameters.SetByCallerMagnitudes)
		{
			GESpec.Data->SetSetByCallerMagnitude(Elem.DataTag, Elem.Magnitude);
		}

		// const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*GESpec.Data.Get());
		OutHandle.GameplayEffectHandles.Add(GameplayEffectHandle);
	}

	// // Grant the attribute sets.
	// for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	// {
	// 	const FFPGrantedAttributeSet& SetToGrant = GrantedAttributes[SetIndex];
	//
	// 	if (!IsValid(SetToGrant.AttributeSet))
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
	// 		continue;
	// 	}
	//
	// 	UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
	// 	ASC->AddAttributeSetSubobject(NewSet);
	// 	OutHandle.GrantedAttributeSets.Add(NewSet);
	// }

	return OutHandle;
}

void FFPAbilitySet::RemoveAbilitySet(FFPAbilitySetHandle& AbilitySetHandle)
{
	if (!AbilitySetHandle.IsValid())
	{
		return;
	}

	if (!AbilitySetHandle.AbilitySystemComponent->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySetHandle.AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySetHandle.AbilitySystemComponent->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : AbilitySetHandle.GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySetHandle.AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
		}
	}

	// for (auto Set : AbilitySetHandle.GrantedAttributeSets)
	// {
	// 	if (Set.IsValid())
	// 	{
	// 		AbilitySetHandle.AbilitySystemComponent->RemoveSpawnedAttribute(Set.Get());
	// 	}
	// }

	AbilitySetHandle.Reset();
}

void UFPAbilitySetLibrary::RemoveAbilitySet(FFPAbilitySetHandle& AbilitySetHandle)
{
	AbilitySetHandle.RemoveAbilitySet();
}
FFPAbilitySetHandle UFPAbilitySetLibrary::GiveAbilitySet(const FFPAbilitySet& AbilitySet, UAbilitySystemComponent* AbilitySystem, UObject* OverrideSourceObject)
{
	return AbilitySet.GiveAbilitySetTo(AbilitySystem, OverrideSourceObject);
}
